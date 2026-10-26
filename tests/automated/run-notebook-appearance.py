#!/usr/bin/env python3
"""Compile and run the installed Notebook redisplay regression."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import signal
import subprocess
import time


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("Notebook regression currently requires Linux/Xvfb")
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
    parser.error("--out must be fresh and separate from --prefix")
output.mkdir(parents=True)
(output / "tmp").mkdir()
source = Path(__file__).with_name("check-notebook-appearance.c").resolve()
binary = output / "check-notebook-appearance"
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"), PYTHONDONTWRITEBYTECODE="1")
env.pop("DISPLAY", None)
library = (prefix / "lib/libXm.so").resolve(strict=True)
receipt = {"library": str(library), "library_sha256": sha256(library), "source": str(source), "source_sha256": sha256(source),
           "prefix": str(prefix), "output": str(output), "steps": [],
           "success": False}
started = time.monotonic()


def run(name, command):
    log = output / (name + ".log")
    began = time.monotonic()
    with log.open("w") as stream:
        process = subprocess.Popen(command, cwd=output, env=env, stdout=stream,
                                   stderr=subprocess.STDOUT, start_new_session=True)
        try:
            process.wait(timeout=180)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
            receipt["steps"].append({"name": name, "exit": 124,
                                     "seconds": round(time.monotonic() - began, 3),
                                     "log": str(log)})
            raise RuntimeError(name + " exceeded its 180-second timeout")
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
    run("link", ["ldd", binary])
    run("run", ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", binary])
    if "libXm.so" not in (output / "link.log").read_text():
        raise RuntimeError("regression did not link libXm")
    if "PASS Notebook connected page flaps" not in (output / "run.log").read_text():
        raise RuntimeError("regression did not report its pass marker")
    if str(prefix / "lib") not in (output / "link.log").read_text():
        raise RuntimeError("regression loaded an unexpected library")
    if sha256(library) != receipt["library_sha256"]:
        raise RuntimeError("library changed during qualification")
    receipt["success"] = True
finally:
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    receipt["seconds"] = round(time.monotonic() - started, 3)
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

print("PASS Notebook connected page flaps")
