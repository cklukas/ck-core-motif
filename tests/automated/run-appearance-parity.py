#!/usr/bin/env python3
"""Compare startup and live appearance on native widgets and gadgets."""
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
parser.add_argument("--build", type=Path, required=True)
parser.add_argument("--out", type=Path, required=True)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("native acceptance requires Linux and Xvfb")
if not args.build.is_absolute() or not args.out.is_absolute() or args.out.exists():
    parser.error("build must be absolute and output must be a fresh absolute directory")
build = args.build.resolve(strict=True)
root = Path(__file__).resolve().parents[2]
source = root / "tests/automated/check-appearance-parity.c"
library = (build / "lib/Xm/.libs/libXm.so").resolve(strict=True)
output = args.out
output.mkdir(parents=True)
(output / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(output / "tmp"),
           LD_LIBRARY_PATH=str(library.parent), PYTHONDONTWRITEBYTECODE="1")
for key in ("LD_PRELOAD", "LD_AUDIT", "XENVIRONMENT", "XAPPLRESDIR", "XUSERFILESEARCHPATH"):
    env.pop(key, None)
def digest(path): return hashlib.sha256(path.read_bytes()).hexdigest()
receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
           "source_sha256": digest(source), "appearance_sha256": digest(root / "lib/Xm/Appearance.c"),
           "library": str(library), "library_sha256": digest(library),
           "steps": [], "success": False}
binary = output / "check-appearance-parity"
started = time.monotonic()
def run(name, command):
    before = time.monotonic()
    with (output / (name + ".log")).open("w") as log:
        process = subprocess.Popen([str(x) for x in command], env=env,
            stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            code = process.wait(timeout=120)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
            code = 124
    receipt["steps"].append({"name": name, "command": [str(x) for x in command],
                             "exit": code, "seconds": time.monotonic() - before})
    if code:
        raise RuntimeError((output / (name + ".log")).read_text())
try:
    run("compile", ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(build / "lib"), "-I" + str(root / "lib"), source,
        "-L" + str(library.parent), "-Wl,-rpath," + str(library.parent),
        "-lXm", "-lXt", "-lX11", "-o", binary])
    run("link", ["ldd", binary])
    if str(library.parent) not in (output / "link.log").read_text():
        raise RuntimeError("fixture did not load the selected build")
    for profile in ("light", "dark", "contrast"):
        run(profile, ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", binary, profile])
    if digest(library) != receipt["library_sha256"]:
        raise RuntimeError("library changed during qualification")
    receipt["success"] = True
finally:
    receipt["elapsed_seconds"] = time.monotonic() - started
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
print("PASS native appearance parity: light, dark, contrast; widgets, gadgets, explicit overrides")
