import sqlite3
from pathlib import Path
from flask import current_app, g

tableColumns = []


def readTableColumns():
    with open(Path(__file__).parent.parent / "CountEntity/classInfo.cpp") as f:
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


readTableColumns()
tableColumnsNoCase = list(map(lambda x: x.lower(), tableColumns))


def get_db():
    if "db" not in g:
        g.db = sqlite3.connect(
            current_app.config["DATABASE"], detect_types=sqlite3.PARSE_DECLTYPES
        )
        g.db.row_factory = sqlite3.Row
    return g.db


def close_db(e=None):
    db = g.pop("db", None)

    if db is not None:
        db.close()
