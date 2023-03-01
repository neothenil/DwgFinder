import os
import json
import logging
import threading
import sqlite3
import psutil
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

db_path = config["db_path"]
db_table = config["db_table"]
watched_path = config["watched_path"]
zwcad_path = Path(config["zwcad_exe"])
zrx_path = Path(__file__).parent / "bin" / "CountEntity.zrx"
zwcad_args = config["zwcad_args"] + ["/ld", f'"{zrx_path}"']
zwcad_window_title = config["zwcad_window_title"]
worker_time_interval = config["worker_time_interval"]

filesToScan = []
filesToScanLocker = threading.Lock()


@contextmanager
def acquire_file_list():
    """
    Always use this function to manipulate `filesToScan` global variable.
    """
    filesToScanLocker.acquire()
    try:
        yield filesToScan
    finally:
        filesToScanLocker.release()


def execute_sql(sql: str) -> List[Tuple]:
    db = sqlite3.connect(db_path)
    cur = db.cursor()
    cur.execute(sql)
    res = cur.fetchall()
    db.commit()
    db.close()
    return res


def find_exe(name: str) -> Union[bool, psutil.Process]:
    for proc in psutil.process_iter():
        if proc.name().lower() == name.lower():
            return proc
    return False
