#!/usr/bin/env python3
"""Compile and run the installed XmTabBox redisplay regression."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import time


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("XmTabBox regression currently requires Linux/Xvfb")
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
    parser.error("--out must be fresh and separate from --prefix")
output.mkdir(parents=True)
(output / "tmp").mkdir()
source = Path(__file__).with_name("check-tab-box-redisplay.c").resolve()
binary = output / "check-tab-box-redisplay"
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"), PYTHONDONTWRITEBYTECODE="1")
env.pop("DISPLAY", None)
receipt = {"source": str(source), "source_sha256": sha256(source),
           "prefix": str(prefix), "output": str(output), "steps": [],
           "success": False}
started = time.monotonic()


def run(name, command):
    log = output / (name + ".log")
    began = time.monotonic()
    with log.open("w") as stream:
        process = subprocess.run(command, cwd=output, env=env, stdout=stream,
                                 stderr=subprocess.STDOUT, timeout=120)
    receipt["steps"].append({"name": name, "command": [str(item) for item in command],
                             "exit": process.returncode,
                             "seconds": round(time.monotonic() - began, 3),
                             "log": str(log)})
    if process.returncode:
        print(log.read_text(errors="replace"), end="")
        raise SystemExit(process.returncode)


try:
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
             "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
             "-Wl,-rpath," + str(prefix / "lib")]
    run("compile", ["cc", *flags, source, "-lXm", "-lXt", "-lX11",
                     "-o", binary])
    linked = run("link", ["ldd", binary])
    run("run", ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", binary])
    if "libXm.so.8" not in (output / "link.log").read_text():
        raise RuntimeError("regression did not link libXm")
    if "PASS XmTabBox live appearance redisplay" not in (output / "run.log").read_text():
        raise RuntimeError("regression did not report its pass marker")
    receipt["success"] = True
finally:
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    receipt["seconds"] = round(time.monotonic() - started, 3)
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

print("PASS XmTabBox live appearance redisplay")
