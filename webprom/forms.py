from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, BooleanField, SelectField, SubmitField, DateField, FileField
from wtforms.validators import DataRequired


class LoginForm(FlaskForm):
    username = StringField('Пользователь', validators=[DataRequired()])
    password = PasswordField('Пароль', validators=[DataRequired()])
    submit = SubmitField('Войти')


class OnDateForm(FlaskForm):
    date = DateField('на дату', validators=[DataRequired()])
    submit = SubmitField('пересчитать')


class FromToDateForm(FlaskForm):
    since = DateField('с', validators=[DataRequired()])
    till = DateField('по', validators=[DataRequired()])
    submit = SubmitField('пересчитать')

class FromToGoodsForm(FromToDateForm):
    goods = SelectField('культура', choices=[])

class RepCheck681Form(FromToDateForm):
    file = FileField('файл для импорта', validators=[DataRequired()])
    account = SelectField('счёт 1С', choices=[(0, '6442'), (1, '6432')])