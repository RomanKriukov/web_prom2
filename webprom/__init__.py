import os
from flask import Flask
from flask_cors import CORS
from flask_wtf import CSRFProtect
from datetime import timedelta
from config import Config, ConfigApp
from webprom.routes_main import route_main
from webprom.routes_api import route_api
from webprom.routes_reports import route_reports


app = Flask(__name__)
app.config.from_object(ConfigApp)
app.permanent_session_lifetime = timedelta(minutes=Config.SESSION_LIFETIME_MINUTES)
app.register_blueprint(route_main)
app.register_blueprint(route_api)
app.register_blueprint(route_reports)
CORS(app, origins="*", supports_credentials=True, methods=['GET', 'POST', 'OPTIONS'])
# CORS(app, origins=["http://127.0.0.1", "http://192.168.0.3"], supports_credentials=True, methods=['GET', 'POST', 'OPTIONS'], resources={r"/api/*": {"origins": ["http://127.0.0.1", "http://192.168.0.3"]}})
csrf = CSRFProtect(app)
app.debug = True

if not os.path.exists(app.config['UPLOAD_FOLDER']):
    os.makedirs(app.config['UPLOAD_FOLDER'])

# origins="*" - DEBUG!!!

# DEL import webprom.routes