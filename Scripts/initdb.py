import os
import sqlite3
import json
from pathlib import Path

config_file = Path(os.getcwd()) / "config.json"
with open(config_file, encoding="utf8") as f:
    config = json.load(f)

dbPath = config["db_path"]
tableName = config["db_table"]
tableColumns = config["db_columns"]


def createTable():
    db = sqlite3.connect(dbPath)
    cur = db.cursor()
    cur.execute(f"DROP TABLE IF EXISTS {tableName}")
    strColumns = "id INTEGER PRIMARY KEY AUTOINCREMENT," + "path TEXT UNIQUE"
    for clsName in tableColumns:
        strColumns += f",{clsName} INTEGER NOT NULL"
    strSql = f"CREATE TABLE {tableName} ({strColumns});"
    cur.execute(strSql)
    db.close()


if __name__ == "__main__":
    createTable()
