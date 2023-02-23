import os
import sqlite3
import json
from pathlib import Path

config_file = Path(os.getcwd()) / "config.json"
with open(config_file) as f:
    config = json.load(f)

dbPath = config["db_path"]
tableName = config["db_table"]
tableColumns = []


def readTableColumns():
    with open("CountEntity/classInfo.cpp") as f:
        lines = f.readlines()
    startLine = 0
    for lineNum, line in enumerate(lines):
        if "gClassNames" in line:
            startLine = lineNum + 1
            break
    i = startLine
    while "};" not in lines[i]:
        if lines[i].strip(' ",\r\n').startswith("//"):
            i += 1
            continue
        className = lines[i].strip(' ",\r\n')
        tableColumns.append(className)
        i += 1


def createTable():
    db = sqlite3.connect(dbPath)
    cur = db.cursor()
    cur.execute(f"DROP TABLE IF EXISTS {tableName}")
    readTableColumns()
    strColumns = "id INTEGER PRIMARY KEY AUTOINCREMENT," + "path TEXT UNIQUE"
    for clsName in tableColumns:
        strColumns += f",{clsName} INTEGER NOT NULL"
    strSql = f"CREATE TABLE {tableName} ({strColumns});"
    cur.execute(strSql)
    db.close()


if __name__ == "__main__":
    createTable()
