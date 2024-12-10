from flask import current_app
from sqlalchemy.exc import DBAPIError
from sqlalchemy.orm import Session
from decimal import Decimal
import pandas as pd
import numpy as np

def add_bar(x, y, label, items_list):
    y = round(y)
    items_list.append(            
            {
                'x': x,
                'y': y,
                'indexLabel': '' if y==0 else str(y),
                'label': label
            }
        )

def rep_elevators_input(session: Session, date_since: str, date_till: str, goods: str):
    try:
        connection = session.connection().connection
        cursor = connection.cursor()
        query = f"EXEC repElevatorsInput '{date_since.strftime("%Y%m%d")}', '{date_till.strftime("%Y%m%d")}', {"'"+goods+"'" if goods else 'NULL'}"
        cursor.execute(query)

        totals_data = cursor.fetchall()      # dataset с итогами
        totals_columns = [desc[0] for desc in cursor.description]
        totals_data = [float(value) if isinstance(value, Decimal) else value for value in totals_data[0]]
        totals = dict(zip(totals_columns, totals_data))
        # current_app.logger.info(totals)

        cursor.nextset()                # пропускаем dataset со списком всех товаров в базе
        
        cursor.nextset()
        data = cursor.fetchall()        # dataset с основными данными 
        data_columns = [desc[0] for desc in cursor.description]
        data = [[float(value) if isinstance(value, Decimal) else value for value in row] for row in data]        
        df = pd.DataFrame(data, columns=data_columns)
    except DBAPIError as e:
        return e._sql_message()
    except Exception as e:
        return e        
    finally:
        session.close()
        if cursor:
            cursor.close()
    
    items1 = []
    items2 = []
    items3 = []

    for index, row in df.iterrows():
        nal = row['amount']-row['beznal']-row['farms']
        percent_beznal = round(row['beznal']/row['amount']*100,1)
        percent_nal = round(nal/row['amount']*100,1)
        percent_farms = round(row['farms']/row['amount']*100,1)
        label = f"{row['elevatorname']} ({percent_beznal} %/{percent_nal} %/{percent_farms})"
        add_bar(index, row['beznal'], label, items1)
        add_bar(index, nal, label, items2)
        add_bar(index, row['farms'], label, items3)

    res = [totals, items1, items2, items3]
    return res