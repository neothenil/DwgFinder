import sqlite3
from typing import List, Tuple
from watchdog.events import PatternMatchingEventHandler
from watchdog.observers import Observer

from .common import logger, db_table
from .common import acquire_file_list
from .common import watched_path
from .common import execute_sql


class DwgFileEventHanlder(PatternMatchingEventHandler):
    def __init__(self):
        super(DwgFileEventHanlder, self).__init__(
            ["*.dwg", "*.dxf"], case_sensitive=False
        )

    def on_created(self, event):
        logger.info("Watchdog received created event - %s." % event.src_path)
        self.append_to_file_list(event.src_path)

    def on_modified(self, event):
        """
        Do not handle modified event. It's buggy on Windows. Or use hash value to
        detect content change in future.
        """
        # logger.info("Watchdog received modified event - % s." % event.src_path)
        # self.delete_record(event.src_path)
        # self.append_to_file_list(event.src_path)

    def on_deleted(self, event):
        logger.info("Watchdog received deleted event - %s." % event.src_path)
        self.delete_record(event.src_path)

    def on_moved(self, event):
        logger.info("Watchdog received moved event - %s." % event.src_path)
        self.delete_record(event.src_path)
        self.append_to_file_list(event.dest_path)

    def delete_record(self, path):
        path = path.lower()
        # search from file list
        with acquire_file_list() as file_list:
            if path in file_list:
                file_list.remove(path)
                logger.debug("filesToScan = " + str(file_list))
                return
        # search from db
        res = execute_sql(f'select id from {db_table} where path = "{path}";')
        if len(res) == 0:
            return
        execute_sql(f'delete from {db_table} where path = "{path}";')

    def append_to_file_list(self, path):
        path = path.lower()
        with acquire_file_list() as file_list:
            if path not in file_list:
                file_list.append(path)
                logger.debug("filesToScan = " + str(file_list))


def start_watcher() -> Observer:
    """
    Start observer and return the Thread object.
    Remember to call `stop` and `join` on the Thread object.
    """
    event_handler = DwgFileEventHanlder()
    observer = Observer()
    observer.schedule(event_handler, watched_path, recursive=True)
    observer.start()
    return observer
