from flask import session, current_app
from sqlalchemy import create_engine, text
from sqlalchemy.orm import sessionmaker
from datetime import datetime, timedelta
from threading import Timer
from config import Config


def create_mssql_session_engine(username: str, password: str) -> str:       
    db_uri = f"mssql+pyodbc://{username}:{password}@{Config.SQL_HOST}:{Config.SQL_PORT}/{Config.SQL_DB}?TrustServerCertificate=yes&Encrypt=no&driver={Config.SQL_DRIVER}"    
    engine = create_engine(db_uri, connect_args={'timeout': 60}, pool_recycle=60, pool_pre_ping=True, pool_size=100, max_overflow=50, isolation_level="AUTOCOMMIT")       
    Session = sessionmaker(bind=engine)  
    try:        
        query = text(f"SELECT * FROM dbo.userRoles('{username}')")        
        rs = Session().execute(query).fetchall()        
        roles = [role[0] for role in rs]
        # current_app.logger.info(roles)
        session['username'] = username
        activity = datetime.now()
        Config.session_engine[username] = (Session, roles, activity)
        return 'Login successful!'
    except Exception as e:
        current_app.logger.info(e)
        engine  = None
        return 'Login error!'
    
def close_mssql_session_engine() -> str:
    try:
        db_session, *_ = Config.session_engine.pop(session['username'], (None,)*3)
        if db_session:
            db_session.close_all()
        session.pop('username', None)
        return 'Logout successful'
    except:
        return 'Logout failed'
    
def update_session_activity(username: str):
    db_session, roles, _ = Config.session_engine.get(username, (None,)*3)
    if db_session:
        Config.session_engine[username] = (db_session, roles, datetime.now())    
    
def close_inactive_sessions():
    now = datetime.now()
    session_timeout = timedelta(minutes=Config.SESSION_LIFETIME_MINUTES)
    for username, (db_session, roles, activity) in list(Config.session_engine.items()):
        if (now - activity) > session_timeout:
            db_session.close_all()
            # current_app.logger.info(f"CLOSED: {username}")
            Config.session_engine.pop(username)

    # t = Timer(60, close_inactive_sessions)
    # t.start()

def first_day() -> datetime:
    return datetime.now().replace(day=1)

def last_day() -> datetime:
    # 28й день текущего месяца + 4 дня (чтоб точно быть в следующем месяце), потом меняем день на 1й и отнимаем один день
    return (datetime.now().replace(day=28) + timedelta(days=4)).replace(day=1) - timedelta(days=1) 
    

# import jwt
# from datetime import datetime, timedelta
# from app import app

# def generate_report_token(username):
#     payload = {
#         'report': username,
#         'exp': datetime.now() + timedelta(hours=1)  # Токен дійсний протягом 1 години
#     }
#     # print(datetime.now())
#     # print(datetime.now() + timedelta(hours=1))
#     token = jwt.encode(payload, app.config['SECRET_KEY'], algorithm='HS256')
#     return token

# def decode_report_token(token):
#     try:
#         payload = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
#         return payload['report']    
#     # except jwt.ExpiredSignatureError:
#     #     return 'Токен прострочений'
#     # except jwt.InvalidTokenError:
#     #     return 'Неправильний токен'
#     except:
#         return None