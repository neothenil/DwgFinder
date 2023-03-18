import os
import json
from pathlib import Path

config_file = Path(os.getcwd()) / "config.json"
with open(config_file, encoding="utf8") as f:
    config = json.load(f)
