#from webprom import app
#from config import Config


#if  __name__ == '__main__':
#    app.run(debug=True, host='0.0.0.0', port=Config.FLASK_PORT)

import cherrypy
from webprom import app
from config import Config

class FlaskAppWrapper:
    def __init__(self):
        self.app = app

    def __call__(self, environ, start_response):
        return self.app(environ, start_response)

if __name__ == '__main__':
    cherrypy.tree.graft(FlaskAppWrapper(), '/')
    cherrypy.server.unsubscribe()

    server = cherrypy._cpserver.Server()
    server.socket_host = '0.0.0.0'
    server.socket_port = Config.FLASK_PORT
    server.thread_pool = 30

    server.subscribe()
    cherrypy.engine.start()
    cherrypy.engine.block()