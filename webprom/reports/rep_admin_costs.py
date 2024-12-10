import pandas as pd
import numpy as np
from sqlalchemy import text
from sqlalchemy.exc import DBAPIError
from sqlalchemy.orm import Session


def rep_admin_costs(session: Session, date_since: str, date_till: str) -> str:
    try:
        query = f"EXEC repAdminCosts '{date_since.strftime("%Y%m%d")}', '{date_till.strftime("%Y%m%d")}'"
        rs = session.execute(text(query)).fetchall()
        df = pd.DataFrame(rs)
    except DBAPIError as e:
        return e._sql_message()
    except Exception as e:
        return e    
    finally:
        session.close()      

    df = df.astype({
        'deb': 'float64'
    }).round(0)    

    df.replace(np.nan, '', inplace=True)

    columns_to_show = ['name', 'deb']
    columns_name = ['Наименование', 'Сумма']
    columns_width = [50, 50]
    name = dict(zip(columns_to_show, columns_name))
    width = dict(zip(columns_to_show, columns_width))

    html = '<table class="treetable" width="98%">\n'

    html += '<thead><tr>'
    for col in df.columns:        
        html += f'<th style="width:{width[col]}%">{name[col]}</th>' if col in columns_to_show else ''
    html += '</tr></thead>\n'

    for index, row in df.iterrows():
        html += f'<tr class="lev{row["levl"]+2}">'                 
        for col in df.columns: 
            if col in columns_to_show:
                if col == 'name' and row["levl"] <= 0 and row["acc"] not in (1000, 1100, 1101, 1104):
                    html += f'<td><label><input type="checkbox"><a onclick="sh(this)">{row[col]}</a></label></td>'
                else:
                    html += f'<td>{int(row[col]):,}</td>'.replace(",", " ") if type(row[col]) == float else f'<td>{row[col]}</td>'
        html += '</tr>\n'

    html += '</table>'

    return html
