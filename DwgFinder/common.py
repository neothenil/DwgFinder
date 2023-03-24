import os
import json
import logging
import threading
import sqlite3
from pathlib import Path
from typing import List, Tuple, Union
from contextlib import contextmanager


config_file = Path(os.getcwd()) / "config.json"
with open(config_file, encoding="utf8") as f:
    config = json.load(f)

logging.basicConfig(
    level=logging.INFO,
    format="[%(levelname)s] %(asctime)s - %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
    handlers=[
        logging.FileHandler("dwgfinder.log", mode="w"),
        logging.StreamHandler(),
    ],
)
logger = logging.getLogger()
loggerLock = threading.Lock()

db_path = config["db_path"]
db_table = config["db_table"]
watched_path = config["watched_path"]
worker_time_interval = config["worker_time_interval"]
dwgread_path = Path(__file__).parent / "bin" / "DwgRead.exe"

filesToScan = []
filesToScanLock = threading.Lock()


@contextmanager
def acquire_file_list():
    """
    Always use this function to manipulate `filesToScan` global variable.
    """
    filesToScanLock.acquire()
    try:
        yield filesToScan
    finally:
        filesToScanLock.release()


@contextmanager
def acquire_logger():
    loggerLock.acquire()
    try:
        yield logger
    finally:
        loggerLock.release()


def execute_sql(sql: str) -> List[Tuple]:
    db = sqlite3.connect(db_path)
    cur = db.cursor()
    cur.execute(sql)
    res = cur.fetchall()
    db.commit()
    db.close()
    return res
