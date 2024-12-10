from flask import session, redirect, render_template, flash, url_for, Blueprint, current_app, send_from_directory, jsonify
from config import Config
from webprom.utils import create_mssql_session_engine, close_mssql_session_engine, close_inactive_sessions
from webprom.forms import LoginForm


route_main = Blueprint('route_main', __name__)

@route_main.before_request
def before_request():
    session.permanent = True
    close_inactive_sessions()

@route_main.route('/uploads/<filename>')
def upload_file(filename):
    return send_from_directory(current_app.config['UPLOAD_FOLDER'],
                               filename)

@route_main.route('/download/<filename>')
def download_file(filename):
    return send_from_directory(f"../{current_app.config['UPLOAD_FOLDER']}", 
                               filename, 
                               as_attachment=True)

@route_main.route('/', methods=['GET', 'POST'])
@route_main.route('/login', methods=['GET', 'POST'])
def login():
    templ_file_name = 'login'
    if 'username' in session and Config.session_engine.get(session['username']):
        return redirect(url_for('route_main.index'))
    else:        
        form = LoginForm()
        if form.validate_on_submit():   
            flash(create_mssql_session_engine(form.username.data, form.password.data))
            return redirect(url_for('route_main.index'))
        else:
            current_app.logger.info(form.errors)
            return render_template(            
                f"{templ_file_name}.html", 
                style=f"{templ_file_name}.css",
                title='Вход', 
                form=form
            )
    
@route_main.route('/logout')
def logout():
    flash(close_mssql_session_engine())  
    return redirect(url_for('route_main.login'))

@route_main.route('/index')
def index():
    return render_template('index.html', title='Главная', login=session.get('username'))