import os
import re
from flask import session, redirect, render_template, url_for, Blueprint, current_app, request
from datetime import datetime
from sqlalchemy import text
from sqlalchemy.exc import DBAPIError
from werkzeug.utils import secure_filename
from pathlib import Path
from config import Config
from webprom.utils import update_session_activity, first_day, last_day
from webprom.forms import OnDateForm, FromToDateForm, FromToGoodsForm, RepCheck681Form
from webprom.reports.rep_exp_nds import rep_exp_nds
from webprom.reports.rep_admin_costs import rep_admin_costs
from webprom.reports.rep_elevators_input import rep_elevators_input
from webprom.reports.rep_check_681 import rep_check_681


route_reports = Blueprint('route_reports', __name__)

@route_reports.route('/rep-check-681', methods=['GET', 'POST'])
def exec_rep_check_681():
    if 'username' in session and Config.session_engine.get(session['username']):
        db_session, *_ = Config.session_engine.get(session['username'])
        form = RepCheck681Form()
        if not form.since.data:
            form.since.data = first_day()
        if not form.till.data:            
            form.till.data = last_day() 
        res = ['', None]
        if request.method == 'POST':
            file = request.files['file']
            if file:
                filename = file.filename
                filename = filename.strip().replace(" ", "_")
                filename = re.sub(r'(?u)[^-\w.\u0400-\u04FF]', '', filename)
                # filename = secure_filename(file.filename)
                filepath = os.path.join(current_app.config['UPLOAD_FOLDER'], filename)
                file.save(filepath)
                try:
                    serverpath = f"{Config.NETWORK_PATH}\\{current_app.config['UPLOAD_FOLDER']}\\{filename}"
                    # current_app.logger.info(serverpath)             
                    res = rep_check_681(db_session(), form.since.data, form.till.data, serverpath, form.account.data)
                finally:
                    os.remove(filepath) 
        update_session_activity(session['username'])            
        return render_template(
            'rep-check-681.html',
            style='levels.css',
            script='levels.js',
            title='Сверка по 681 показателю',
            form=form,
            table=res[0],
            file=res[1],
        )
    return redirect(url_for('route_main.login'))

@route_reports.route('/rep-elevators-input', methods=['GET', 'POST'])
def exec_rep_elevators_input(): 
    if 'username' in session and Config.session_engine.get(session['username']):
        db_session, *_ = Config.session_engine.get(session['username'])
        form = FromToGoodsForm()
        form.goods.label.text = 'культура'
        try:
            goods_list = db_session().execute(text(f"SELECT id, name FROM goods WHERE type IN (3,4,5,6) ORDER BY type, name")).fetchall() 
        except DBAPIError as e:
            return e._sql_message()
        except Exception as e:
            return e                   
        form.goods.choices = [tuple(goods) for goods in goods_list]        
        if not form.since.data:
            form.since.data = first_day()
        if not form.till.data:            
            form.till.data = last_day() 
        if not form.goods.data:
            form.goods.data = '1BC93D91-BCA0-460B-9874-719DE5554B6D'
        update_session_activity(session['username'])
        res = rep_elevators_input(
            session=db_session(), 
            date_since=form.since.data,
            date_till=form.till.data,
            goods=form.goods.data
        )
        return render_template(
            'rep-elevators-input.html', 
            style='levels.css',
            script='canvasjs.min.js',
            title='Поступления на элеваторы (нал/безнал) + Хозяйства', 
            form=form, 
            totals=res[0],
            items1=res[1],
            items2=res[2],
            items3=res[3]
        )
    return redirect(url_for('route_main.login'))

@route_reports.route('/rep-admin-costs', methods=['GET', 'POST'])
def exec_rep_admin_costs(): 
    templ_file_name = 'rep-admin-costs'
    if 'username' in session and Config.session_engine.get(session['username']):
        db_session, *_ = Config.session_engine.get(session['username'])
        form = FromToDateForm()        
        if not form.since.data:
            form.since.data = first_day()
        if not form.till.data:            
            form.till.data = last_day()           
        update_session_activity(session['username'])
        return render_template(
            f"{templ_file_name}.html", 
            style='levels.css',
            style2=f"{templ_file_name}.css",
            script='levels.js',
            title='Админ. расходы', 
            form=form, 
            table=rep_admin_costs(
                session=db_session(), 
                date_since=form.since.data,
                date_till=form.till.data
            )
        )
    return redirect(url_for('route_main.login'))

@route_reports.route('/rep-exp-nds', methods=['GET', 'POST'])
def exec_rep_exp_nds(): 
    if 'username' in session and Config.session_engine.get(session['username']):
        db_session, *_ = Config.session_engine.get(session['username'])
        form = OnDateForm()        
        if not form.date.data:
            form.date.data = datetime.now()
        update_session_activity(session['username'])
        return render_template(
            'rep-exp-nds.html', 
            style='levels.css',
            script='levels.js',
            title='Просроченный НДС', 
            form=form, 
            table=rep_exp_nds(
                session=db_session(), 
                rep_date=form.date.data
            )
        )
    return redirect(url_for('route_main.login'))