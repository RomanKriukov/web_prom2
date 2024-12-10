import os
import numpy as np


class ConfigApp(object):
    SECRET_KEY = os.environ.get('SECRET_KEY') or ''.join(np.random.choice(np.array([chr(i) for i in range(48,123)]), size=np.random.randint(20, 30)))
    WTF_CSRF_SECRET_KEY = os.environ.get('WTF_CSRF_SECRET_KEY') or ''.join(np.random.choice(np.array([chr(i) for i in range(48,123)]), size=np.random.randint(20, 30)))
    WTF_CSRF_SSL_STRICT = bool(os.environ.get('WTF_CSRF_SSL_STRICT', False))
    SESSION_COOKIE_SAMESITE = os.environ.get('SESSION_COOKIE_SAMESITE', 'Lax')#None')
    SESSION_COOKIE_SECURE = bool(os.environ.get('SESSION_COOKIE_SECURE', False))#True)) #True for HTTPS
    SESSION_TYPE = os.environ.get('SESSION_TYPE', 'filesystem')
    SESSION_FILE_DIR = os.environ.get('SESSION_FILE_DIR', './.flask_session/')   
    UPLOAD_FOLDER = os.environ.get('UPLOAD_FOLDER', 'uploads') 


class Config(object):
    SQL_HOST = os.environ.get('SQL_HOST', 'prometey.net.ua')
    SQL_PORT = os.environ.get('SQL_PORT', '1433')
    SQL_DRIVER = os.environ.get('SQL_DRIVER', 'ODBC+Driver+17+for+SQL+Server')
    SQL_DB = os.environ.get('SQL_DB', 'fa')

    SESSION_LIFETIME_MINUTES = int(os.environ.get('SESSION_LIFETIME_MINUTES', 60))
    FLASK_PORT = int(os.environ.get('FLASK_PORT', 5000))

    NETWORK_PATH = "\\\\192.168.0.208\\web_prom2"

    session_engine = {}
    # {
    #     '<session_username>': (engine, roles: list(str), last_activity)
    # }