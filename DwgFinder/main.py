import time
from .watcher import start_watcher
from .worker import start_worker


def main():
    watcher = start_watcher()
    worker = start_worker()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        worker.stop()
        watcher.stop()
    worker.join()
    watcher.join()
