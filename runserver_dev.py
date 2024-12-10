from webprom import app
from config import Config


if  __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=Config.FLASK_PORT)