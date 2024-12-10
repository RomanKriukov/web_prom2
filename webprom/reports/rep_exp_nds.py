import pandas as pd
import numpy as np
from datetime import date
from sqlalchemy import text
from sqlalchemy.exc import DBAPIError
from sqlalchemy.orm import Session


def rep_exp_nds(session: Session, rep_date: str) -> str:
    try:
        query = f"EXEC repExpNds '{(date.today() if rep_date == None else rep_date).strftime("%Y%m%d")}'"
        rs = session.execute(text(query)).fetchall()
        df = pd.DataFrame(rs)
    except DBAPIError as e:
        return e._sql_message()
    except Exception as e:
        return e 
    finally:
        session.close()          

    df = df.astype({
        'exp_1_30': 'float64',
        'exp_30_90': 'float64',
        'exp_90_360': 'float64',
        'exp_360': 'float64',
    }).round(0)    

    df.replace(np.nan, '', inplace=True)

    columns_to_show = ['name', 'doc', 'exp_1_30', 'exp_30_90', 'exp_90_360', 'exp_360']
    columns_name = ['Наименование', 'Документ', 'Просрочка 1-30 дней', 'Просрочка 30-90 дней', 'Просрочка 90-360 дней', 'Просрочка более 360 дней']
    columns_width = [25, 35, 10, 10, 10, 10]
    name = dict(zip(columns_to_show, columns_name))
    width = dict(zip(columns_to_show, columns_width))

    html = '<table class="treetable" width="98%">\n'

    html += '<thead><tr>'
    for col in df.columns:        
        html += f'<th style="width:{width[col]}%">{name[col]}</th>' if col in columns_to_show else ''
    html += '</tr></thead>\n'

    for index, row in df.iterrows():
        html += f'<tr class="lev{row["levl"]+2}" {"id=""summary"">" if row["summary"] == 1 else ">"}'                 
        for col in df.columns: 
            if col in columns_to_show:
                if col == 'name' and row["levl"] <= 0 and row["summary"] == 0:
                    html += f'<td><label><input type="checkbox"><a onclick="sh(this)">{row[col]}</a></label></td>'
                else:
                    html += f'<td>{int(row[col]):,}</td>'.replace(",", " ") if type(row[col]) == float else f'<td>{row[col]}</td>'
        html += '</tr>\n'

    html += '</table>'

    return html
