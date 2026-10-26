#!/usr/bin/env python3
"""Qualify Periodic's actual startup/UIL hierarchy against an explicit build."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import select
import signal
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--build", type=Path, required=True)
parser.add_argument("--out", type=Path, required=True)
args = parser.parse_args()
if not args.build.is_absolute() or not args.out.is_absolute() or args.out.exists():
    parser.error("build must be absolute and out must be a fresh absolute directory")
root = Path(__file__).resolve().parents[2]
build = args.build.resolve(strict=True)
out = args.out
out.mkdir(parents=True)
(out / "tmp").mkdir()
(out / "app-defaults").mkdir()
# Install the old resource file under its real class to prove class isolation.
(out / "app-defaults/XmdPeriodic").write_bytes((root / "demos/programs/periodic/XmdPeriodic.ad").read_bytes())
env = dict(os.environ, TMPDIR=str(out / "tmp"),
    LD_LIBRARY_PATH=":".join(str(build / p) for p in ("lib/Mrm/.libs", "lib/Xm/.libs")),
    XFILESEARCHPATH=str(out / "app-defaults/%N"), XUSERFILESEARCHPATH=str(out / "missing/%N"))
for key in ("LD_PRELOAD", "LD_AUDIT", "XENVIRONMENT", "XAPPLRESDIR", "UIDPATH"):
    env.pop(key, None)
def digest(path): return hashlib.sha256(path.read_bytes()).hexdigest()
sources = [root / "tests/automated/check-periodic-appearance.c", root / "demos/programs/periodic/periodic.c",
    root / "demos/programs/periodic/periodic.uil", root / "demos/programs/periodic/periodic_l.uil",
    root / "lib/Xm/Appearance.c", root / "lib/Xm/List.c", root / "lib/Xm/XmString.c", Path(__file__)]
libraries = [(build / p).resolve(strict=True) for p in ("lib/Mrm/.libs/libMrm.so", "lib/Xm/.libs/libXm.so")]
receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "sources": {str(p): digest(p) for p in sources},
    "libraries": {str(p): digest(p) for p in libraries},
    "uid_sha256": digest(build / "demos/programs/periodic/periodic.uid"),
    "steps": [], "success": False}
binary = out / "check-periodic-appearance"
cwd = build / "demos/programs/periodic"
def run(name, command, expect=0, directory=cwd):
    start = time.monotonic()
    with (out / (name + ".log")).open("w") as log:
        p = subprocess.Popen([str(x) for x in command], env=env, cwd=directory,
            stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
        try: code = p.wait(timeout=60)
        except subprocess.TimeoutExpired:
            os.killpg(p.pid, signal.SIGKILL)
            p.wait()
            code = 124
    receipt["steps"].append({"name": name, "command": [str(x) for x in command],
        "exit": code, "expected": expect, "seconds": time.monotonic() - start})
    output = (out / (name + ".log")).read_text()
    if code != expect or "Attempt to change XmNshadowThickness" in output:
        raise RuntimeError(output)
    print("PASS", name, flush=True)
server = None
read_fd, write_fd = os.pipe()
try:
    run("compile", ["cc", "-std=c11", "-Wall", "-Wextra", "-Wno-unused-parameter", "-Werror", "-O2",
        "-I" + str(build / "lib"), "-I" + str(root / "lib"), sources[0],
        "-L" + str(libraries[0].parent), "-L" + str(libraries[1].parent),
        "-lMrm", "-lXm", "-lXt", "-lX11", "-o", binary])
    run("link", ["ldd", binary])
    for library in libraries:
        if str(library.parent) not in (out / "link.log").read_text():
            raise RuntimeError("fixture did not load the selected cohort")
    with (out / "xserver.log").open("w") as log:
        server = subprocess.Popen(["Xvfb", "-displayfd", str(write_fd), "-screen", "0", "1600x1200x24",
            "-nolisten", "tcp", "-ac"], env=env, pass_fds=(write_fd,), stdout=log, stderr=subprocess.STDOUT)
    os.close(write_fd); write_fd = -1
    ready_bytes = b""
    deadline = time.monotonic() + 15
    while not ready_bytes.endswith(b"\n"):
        remaining = deadline - time.monotonic()
        if remaining <= 0 or not select.select([read_fd], [], [], remaining)[0]:
            raise RuntimeError("Xvfb did not become ready")
        chunk = os.read(read_fd, 64)
        if not chunk: raise RuntimeError("Xvfb closed its readiness pipe")
        ready_bytes += chunk
    display = ready_bytes.decode().strip()
    if not display.isdigit(): raise RuntimeError("invalid Xvfb ready response")
    env["DISPLAY"] = ":" + display
    for mode, options in (("light", []), ("dark", ["--appearance", "modern-dark"]),
        ("contrast", ["--appearance", "high-contrast"]), ("compact", ["--density", "compact"]),
        ("classic", ["--appearance", "classic"]), ("historical", ["--historical-resources"]),
        ("override", ["-xrm", "*pushButton.background: #006644"])):
        env["PERIODIC_EXPECT"] = mode
        run(mode, [binary] + options)
    run("missing-uid", [binary], expect=1, directory=out)
    run("invalid-profile", [binary, "--appearance", "unknown"], expect=1)
    run("missing-option-value", [binary, "--density"], expect=1)
    run("conflicting-modes", [binary, "--historical-resources", "--appearance", "modern-dark"], expect=1)
    run("help", [binary, "--help"])
    receipt["success"] = True
finally:
    if server is not None:
        server.terminate()
        try: server.wait(timeout=5)
        except subprocess.TimeoutExpired: server.kill(); server.wait()
    os.close(read_fd)
    if write_fd >= 0: os.close(write_fd)
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
