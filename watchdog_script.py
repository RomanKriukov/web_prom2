import os
import sys
import time
import subprocess
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

class RestartServerHandler(FileSystemEventHandler):
    def __init__(self, command, ignore_dirs, ignore_extensions):
        self.command = command
        self.ignore_dirs = ignore_dirs
        self.ignore_extensions = ignore_extensions
        self.process = None
        self.start_server()

    def start_server(self):
        self.process = subprocess.Popen(self.command, shell=True)

    def restart_server(self):
        self.process.terminate()
        self.process.wait()
        self.start_server()

    def on_any_event(self, event):
        for ignore_dir in self.ignore_dirs:
            if ignore_dir in event.src_path:
                return

        _, ext = os.path.splitext(event.src_path)
        if ext.lower() in self.ignore_extensions:
            return
            
        if event.event_type in ('modified', 'created', 'deleted'):
            print(f'File changed: {event.src_path}. Restarting server...')
            self.restart_server()

if __name__ == "__main__":
    path = os.path.dirname(os.path.abspath(__file__))
    command = "python runserver.py"
    ignore_dirs = ['sys', 'uploads', 'static', 'templates', '.git', 'asp']
    ignore_extensions = ['.html', '.css', '.js', '.cmd', '.yml']    # не работает разобраться
    event_handler = RestartServerHandler(command, ignore_dirs, ignore_extensions)
    observer = Observer()
    observer.schedule(event_handler, path, recursive=True)
    observer.start()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()