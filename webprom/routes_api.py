from flask import session, request, jsonify, make_response, Blueprint
from config import Config
from webprom.utils import create_mssql_session_engine, close_mssql_session_engine


route_api = Blueprint('route_api', __name__)

@route_api.route('/api/login', methods=['POST'])
def api_login():
    if not ('username' in session and Config.session_engine.get(session['username'])):
        if request.json and 'username' in request.json and 'password' in request.json:
            msg = create_mssql_session_engine(request.json['username'], request.json['password'])
        else:
            msg = 'Login error!'
    else:
        msg = 'Already logged in'    
    return make_response(jsonify({'msg': msg}), 200)

@route_api.route('/api/logout')
def api_logout():
    msg = close_mssql_session_engine()
    return make_response(jsonify({'msg': msg}), 200)