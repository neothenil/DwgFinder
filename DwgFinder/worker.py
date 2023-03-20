import os
import time
from pathlib import Path
from watchdog.utils import BaseThread as StoppableThread
from datetime import datetime, timedelta
from concurrent.futures import ThreadPoolExecutor
from threading import Semaphore, current_thread

from .reader import Reader
from .common import watched_path, worker_time_interval
from .common import execute_sql
from .common import acquire_file_list, acquire_logger
from .common import db_table, dwgread_path


class WorkingThread(StoppableThread):
    TIME_INTERVAL = timedelta(seconds=worker_time_interval)

    def __init__(self):
        super(WorkingThread, self).__init__()
        self.last_wake_time = None
        self.thread_pool = ThreadPoolExecutor(os.cpu_count() // 4 + 1)
        self.submission_semaphore = Semaphore(self.thread_pool._max_workers)
        self.unknown_failed_files = set()
        self.worker_thread = current_thread()

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
        for dirpath, _, filenames in os.walk(watched_path):
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
                    # self.process_one_file(path)
                    self.submission_semaphore.acquire()
                    try:
                        self.thread_pool.submit(self.process_one_file, path)
                    except RuntimeError:
                        pass  # ignore submit exception after ThreadPoolExecutor has been shut down
        while self.submission_semaphore._value != self.thread_pool._max_workers:
            if not self.should_keep_running():
                return
            # wait submitted task finished
            time.sleep(0.5)
        # retry for files failed by unknown error in the first pass
        for file in self.unknown_failed_files.copy():
            if not self.should_keep_running():
                break
            self.process_one_file(file)
            self.unknown_failed_files.remove(file)

    def process_pending_files(self):
        if not self.should_keep_running():
            return
        with acquire_file_list() as file_list:
            files = file_list.copy()
            file_list.clear()
        for file in files:
            if not self.should_keep_running():
                break
            # self.process_one_file(file)
            self.submission_semaphore.acquire()
            try:
                self.thread_pool.submit(self.process_one_file, file)
            except RuntimeError:
                pass  # ignore submit exception after ThreadPoolExecutor has been shut down
        while self.submission_semaphore._value != self.thread_pool._max_workers:
            if not self.should_keep_running():
                return
            # wait submitted task finished
            time.sleep(0.5)
        # retry for files failed by unknown error in the first pass
        for file in self.unknown_failed_files.copy():
            if not self.should_keep_running():
                break
            self.process_one_file(file)
            self.unknown_failed_files.remove(file)

    def process_one_file(self, path):
        with acquire_logger() as logger:
            logger.info("Processing file %s" % path)
        reader = Reader(dwgread_path, [path])
        reader.execute()
        result = reader.check_result()
        if result[0] != True:
            with acquire_logger() as logger:
                if result[1]:
                    logger.info(f"Failed to process file {path}: {result[1]}")
                else:
                    if path not in self.unknown_failed_files:
                        self.unknown_failed_files.add(path)
                        logger.info(
                            f"Failed to process file {path}. Scheduled to retry later."
                        )
                    else:
                        logger.info(f"Failed to process file {path}")
        if current_thread() != self.worker_thread:
            # only manipulate semaphore when executed in thread pool
            self.submission_semaphore.release()

    def on_thread_stop(self):
        self.thread_pool.shutdown()


def start_worker() -> WorkingThread:
    worker = WorkingThread()
    worker.start()
    return worker
