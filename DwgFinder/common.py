import logging
import threading
import sqlite3
from pathlib import Path
from typing import List, Tuple
from contextlib import contextmanager


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

db_path = "D:\\test.db"
watched_path = "D:\\testFiles\\"
zrx_path = Path(__file__).parent / "bin" / "CountEntity.zrx"
zwcad_path = Path(r"D:\Program Files\ZWSOFT\ZWCAD 2023\ZWCAD.exe")
zwcad_args = [
    "/company",
    "ZWSoft",
    "/product",
    "zwcad",
    "/language",
    "en-US",
    "/ld",
    f'"{zrx_path}"',
]

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
