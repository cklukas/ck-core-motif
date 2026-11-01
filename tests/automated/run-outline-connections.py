#!/usr/bin/env python3
"""Compile and run the installed Outline connector endpoint and repaint regression."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import select
import signal
import subprocess
import time


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
mac = platform.system() == "Darwin"
if platform.system() not in ("Linux", "Darwin"):
    parser.error("outline connections regression requires Linux or macOS/Xvfb")
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
if mac and not any(output.is_relative_to(Path(root)) and Path(root).is_dir()
                   for root in ("/Volumes/S8T/tmp", "/Volumes/PRO-BLADE/tmp")):
    parser.error("macOS --out must be beneath a mounted project temporary volume")
if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
    parser.error("--out must be fresh and separate from --prefix")
output.mkdir(parents=True)
(output / "tmp").mkdir()
source = Path(__file__).with_name("check-outline-connections.c").resolve()
binary = output / "check-outline-connections"
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"), PYTHONDONTWRITEBYTECODE="1")
env.pop("DISPLAY", None)
if mac:
    env.update(DYLD_LIBRARY_PATH=str(prefix / "lib"), LC_ALL="en_US.UTF-8")
library = (prefix / ("lib/libXm.9.dylib" if mac else "lib/libXm.so")).resolve(strict=True)
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


server = None
try:
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
             "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
             "-Wl,-rpath," + str(prefix / "lib")]
    if mac:
        flags += ["-I/opt/X11/include", "-L/opt/X11/lib"]
    run("compile", ["cc", *flags, source, "-lXm", "-lXt", "-lX11",
                     "-o", binary])
    run("link", ["otool", "-L", binary] if mac else ["ldd", binary])
    read_fd, write_fd = os.pipe()
    server = subprocess.Popen(["/opt/X11/bin/Xvfb" if mac else "Xvfb",
        "-displayfd", str(write_fd), "-screen", "0", "1600x1200x24",
        "-noreset", "-nolisten", "tcp"], env=env, pass_fds=(write_fd,),
        stdout=(output / "xvfb.log").open("w"), stderr=subprocess.STDOUT)
    os.close(write_fd)
    with os.fdopen(read_fd) as pipe:
        if not select.select([pipe], [], [], 10)[0]:
            raise RuntimeError("Xvfb did not become ready")
        number = pipe.readline().strip()
        if not number.isdecimal():
            raise RuntimeError("Xvfb did not allocate a display")
        env["DISPLAY"] = ":" + number
    run("run", [binary])
    if "libXm." not in (output / "link.log").read_text():
        raise RuntimeError("regression did not link libXm")
    if "PASS outline connections" not in (output / "run.log").read_text():
        raise RuntimeError("regression did not report its pass marker")
    if str(prefix / "lib") not in (output / "link.log").read_text():
        raise RuntimeError("regression loaded an unexpected library")
    if sha256(library) != receipt["library_sha256"]:
        raise RuntimeError("library changed during qualification")
    receipt["success"] = True
finally:
    if server:
        server.terminate()
        server.wait(timeout=10)
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    receipt["seconds"] = round(time.monotonic() - started, 3)
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

print("PASS outline connections")
