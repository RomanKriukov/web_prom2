import os
import time
import subprocess
from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import psutil

class RestartServerHandler(FileSystemEventHandler):
    def __init__(self, command):
        self.command = command
        self.process = None
        self.start_server()

    def start_server(self):
        print("Starting server...")
        self.process = subprocess.Popen(self.command, shell=True)
        print("Server started.")

    def restart_server(self):
        print("Stopping server...")
        if self.process:
            self.process.terminate()  # Використовуйте terminate замість kill
            try:
                self.process.wait(timeout=10)  # Дочекайтеся завершення процесу
            except subprocess.TimeoutExpired:
                print("Server process did not terminate in time. Killing it...")
                self.process.kill()
                self.process.wait()
            print("Server stopped.")

        # Додамо затримку перед перевіркою порту
        time.sleep(5)  # Затримка в 5 секунд

        # Примусове завершення процесів, що використовують порт 8080
        self.force_kill_port(8080)

        # Додамо затримку, щоб переконатися, що порт звільнився
        self.wait_for_port_free('0.0.0.0', 8080)

        print("Restarting server...")
        self.start_server()

    def wait_for_port_free(self, host, port):
        timeout = 10  # Максимальний час очікування
        start_time = time.time()
        while time.time() - start_time < timeout:
            if not self.is_port_in_use(port):
                print(f'Port {port} is free on {host}.')
                return
            else:
                print(f'Port {port} is still in use. Waiting...')
                time.sleep(1)
        print(f'Port {port} did not become free within the timeout period.')

    def is_port_in_use(self, port):
        """Check if the port is in use by any process"""
        for proc in psutil.process_iter(['pid']):
            try:
                for conn in proc.connections(kind='inet'):
                    if conn.laddr.port == port:
                        return True
            except psutil.NoSuchProcess:
                continue
        return False

    def force_kill_port(self, port):
        """Force kill processes using the specified port"""
        result = subprocess.run(['netstat', '-ano'], capture_output=True, text=True)
        for line in result.stdout.splitlines():
            if f':{port} ' in line:
                pid = line.split()[-1]
                os.system(f'taskkill /F /PID {pid}')

    def on_any_event(self, event):
        if event.event_type in ('modified', 'created', 'deleted'):
            print(f'File changed: {event.src_path}. Restarting server...')
            self.restart_server()

if __name__ == "__main__":
    path = os.path.dirname(os.path.abspath(__file__))
    command = "python runserver.py"
    event_handler = RestartServerHandler(command)
    observer = Observer()
    observer.schedule(event_handler, path, recursive=True)
    observer.start()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
