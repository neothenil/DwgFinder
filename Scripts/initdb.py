import sqlite3
from typing import List

dbPath: str = "D:\\test.db"
tableName: str = "EntityCount"
tableColumns: List[str] = []


def readTableColumns():
    with open("CountEntity/classInfo.cpp") as f:
        lines = f.readlines()
    startLine = 0
    for lineNum, line in enumerate(lines):
        if 'gClassNames' in line:
            startLine = lineNum + 1
            break
    i = startLine
    while '};' not in lines[i]:
        if lines[i].strip(' ",\r\n').startswith('//'):
            i += 1
            continue
        className = lines[i].strip(' ",\r\n')
        tableColumns.append(className)
        i += 1


def createTable():
    db = sqlite3.connect(dbPath)
    cur = db.cursor()
    readTableColumns()
    strColumns = "id INTEGER PRIMARY KEY AUTOINCREMENT," + "path TEXT UNIQUE"
    for clsName in tableColumns:
        strColumns += f",{clsName} INTEGER NOT NULL"
    strSql = f"CREATE TABLE {tableName} ({strColumns});"
    cur.execute(strSql)
    db.close()

if __name__ == "__main__":
    createTable()
