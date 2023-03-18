import os
import zipfile
import tempfile
from pathlib import Path
from flask import (
    Flask,
    render_template,
    redirect,
    url_for,
    request,
    flash,
    send_file,
    abort,
    current_app,
    stream_with_context,
    Response,
)

from .common import config
from .db import get_db, close_db, tableColumns, tableColumnsNoCase


app = Flask(__name__)
app.config.from_mapping(
    SECRET_KEY="fQ_yLyrcjLeyEFUqGw0Izw",
    DATABASE=config["db_path"],
    WATCHED_PATH=Path(config["watched_path"]),
)
app.teardown_appcontext(close_db)


@app.route("/")
def index():
    try:
        query = request.args["q"]
    except:
        query = None
    if query:
        return render_template("index.html", query=query)
    return render_template("index.html")


@app.route("/search")
def search():
    query = request.args["q"]
    if len(query) == 0:
        return redirect(url_for("index"))
    words = query.split()
    columns = ["ID", "Path"]
    for word in words:
        try:
            index = tableColumnsNoCase.index(word.lower())
        except ValueError:
            continue
        else:
            if tableColumns[index] not in columns:
                columns.append(tableColumns[index])
    db = get_db()
    # SQL injection vulnerable
    sql = f"select * from {config['db_table']} where {query};"
    try:
        result = db.execute(sql).fetchall()
    except Exception as e:
        flash(str(e))
        return redirect(url_for("index", q=query))
    return render_template("result.html", query=query, columns=columns, result=result)


@app.route("/download/<int:id>")
def download(id: int):
    db = get_db()
    sql = f"select * from {config['db_table']} where id = {id};"
    try:
        result = db.execute(sql).fetchone()
    except:
        abort(404)
    path = Path(result["Path"])
    if not path.exists():
        abort(404)
    # Python 3.8 doesn't have `is_relative_to` method.
    try:
        path.relative_to(current_app.config["WATCHED_PATH"])
    except:
        abort(404)
    return send_file(path, as_attachment=True)


def clean_temp_files():
    tempdir = Path(tempfile.gettempdir())
    for temp_file in tempdir.glob("DwgFinder-*.zip"):
        try:
            os.remove(tempdir / temp_file)
        except:
            continue


def read_file_chunks(path):
    with open(path, "rb") as f:
        while True:
            buf = f.read(8192)
            if buf:
                yield buf
            else:
                break


@app.route("/download/<string:query>")
def download_by_query(query: str):
    clean_temp_files()
    if len(query) == 0:
        abort(404)
    db = get_db()
    # SQL injection vulnerable
    sql = f"select * from {config['db_table']} where {query};"
    try:
        result = db.execute(sql).fetchall()
    except:
        abort(404)
    fd, temp_path = tempfile.mkstemp(".zip", "DwgFinder-")
    os.close(fd)
    temp_path = Path(temp_path)
    with zipfile.ZipFile(temp_path, "w", zipfile.ZIP_DEFLATED) as temp_zip:
        for row in result:
            path = Path(row["Path"])
            if not path.exists():
                continue
            # Python 3.8 doesn't have `is_relative_to` method.
            try:
                rel_path = path.relative_to(current_app.config["WATCHED_PATH"])
            except:
                continue
            temp_zip.write(path, rel_path)
    response = Response(
        stream_with_context(read_file_chunks(temp_path)), mimetype="application/zlib"
    )
    response.headers["Content-Disposition"] = f"attachment; filename={temp_path.name}"
    return response
