#!/usr/bin/env python3
"""Run the installed deterministic display-scale override fixture."""

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


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("WP-05.2 currently requires Linux/Xvfb")
if not args.prefix.is_absolute() or not args.out.is_absolute():
    parser.error("--prefix and --out must be absolute")
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
    parser.error("--out must be fresh and separate from --prefix")
output.mkdir(parents=True)
(output / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"), PYTHONDONTWRITEBYTECODE="1")
for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
    env.pop(name, None)
source = Path(__file__).resolve().parent / "check-display-scale-override.c"
binary = output / "check-display-scale-override"
receipt = {
    "source": str(source),
    "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
    "prefix": str(prefix),
    "output": str(output),
    "steps": [],
    "success": False,
}
started = time.monotonic()


def run(name, command, timeout=120):
    command = [str(item) for item in command]
    log = output / (name + ".log")
    began = time.monotonic()
    with log.open("w") as stream:
        process = subprocess.Popen(command, cwd=output, env=env, stdout=stream,
                                   stderr=subprocess.STDOUT,
                                   start_new_session=True)
        try:
            code = process.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
            code = 124
    text = log.read_text(errors="replace")
    receipt["steps"].append({"name": name, "command": command, "exit": code,
                             "seconds": round(time.monotonic() - began, 3),
                             "log": str(log)})
    if code:
        print(text, end="")
        raise RuntimeError(f"{name} failed with exit {code}")
    return text


try:
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
             "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
             "-Wl,-rpath," + str(prefix / "lib")]
    run("compile", ["cc", *flags, source, "-lXm", "-lXt", "-lX11",
                     "-o", binary])
    linked = run("link", ["ldd", binary])
    if str(prefix / "lib/libXm.so.8") not in linked:
        raise RuntimeError("audit did not resolve libXm from selected prefix")
    log = run("run", ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24",
                       binary])
    if "PASS WP-05.2 display-scale override" not in log:
        raise RuntimeError("fixture did not report its pass marker")
    receipt["output_sha256"] = hashlib.sha256((output / "run.log").read_bytes()).hexdigest()
    receipt["success"] = True
finally:
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    receipt["seconds"] = round(time.monotonic() - started, 3)
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

print("PASS WP-05.2 display-scale override")
