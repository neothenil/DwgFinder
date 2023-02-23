import os
import psutil
import time
from pathlib import Path
from watchdog.utils import BaseThread as StoppableThread
from datetime import datetime, timedelta

from . import uiautomation as auto

from .zwcad import Zwcad
from .common import watched_path, worker_time_interval
from .common import execute_sql
from .common import zwcad_path, zwcad_args
from .common import acquire_file_list
from .common import logger, db_table


class WorkingThread(StoppableThread):
    TIME_INTERVAL = timedelta(seconds=worker_time_interval)

    def __init__(self):
        super(WorkingThread, self).__init__()
        self.zwcad = None
        self.last_wake_time = None

    def run(self):
        self.walking_watched_path()
        while True:
            if not self.should_keep_running():
                break
            if (
                self.last_wake_time is None
                or datetime.now() - self.last_wake_time >= self.TIME_INTERVAL
            ):
                self.last_wake_time = datetime.now()
                self.process_pending_files()
            else:
                time.sleep(1)

    def walking_watched_path(self):
        if not self.should_keep_running():
            return
        for dirpath, dirnames, filenames in os.walk(watched_path):
            if not self.should_keep_running():
                break
            for file in filenames:
                if not self.should_keep_running():
                    break
                if file.endswith(".dwg") or file.endswith(".dxf"):
                    path = str(Path(dirpath) / file).lower()
                    query_res = execute_sql(
                        f'select id from {db_table} where path = "{path}";'
                    )
                    if len(query_res) != 0:
                        continue
                    self.process_one_file(path)
        if self.zwcad is not None:
            self.zwcad.terminate()
            self.zwcad = None

    def process_pending_files(self):
        if not self.should_keep_running():
            return
        with acquire_file_list() as file_list:
            files = file_list.copy()
            file_list.clear()
        for file in files:
            if not self.should_keep_running():
                break
            self.process_one_file(file)
        if self.zwcad is not None:
            self.zwcad.terminate()
            self.zwcad = None

    def ensure_zwcad_started(self):
        if self.zwcad is None:
            self.start_zwcad()
        if not psutil.pid_exists(self.zwcad.process.pid):
            self.start_zwcad()

    def start_zwcad(self):
        self.zwcad = Zwcad(zwcad_path, zwcad_args)

    def process_one_file(self, path):
        logger.info("Processing file %s" % path)
        with auto.UIAutomationInitializerInThread():
            self.ensure_zwcad_started()
            self.zwcad.invoke(["countEntity", str(path).lower()])
            self.zwcad.wait_process()


def start_worker() -> WorkingThread:
    worker = WorkingThread()
    worker.start()
    return worker
