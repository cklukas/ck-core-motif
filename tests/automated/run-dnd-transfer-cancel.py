#!/usr/bin/env python3
"""Compile and run the installed-prefix DropTransfer cancellation probe."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("run in an isolated Linux task workspace")
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"),
           LC_ALL="C.UTF-8", LD_LIBRARY_PATH=str(args.prefix / "lib"))
source = Path(__file__).resolve().parent / "check-dnd-transfer-cancel.c"
binary = args.results / "check-dnd-transfer-cancel"
receipt = {
    "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "architecture": platform.machine(),
    "prefix": str(args.prefix),
    "source": str(source),
    "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
    "library_sha256": hashlib.sha256(
        (args.prefix / "lib/libXm.so.8.0.0").read_bytes()).hexdigest(),
}
started = time.monotonic()
try:
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
             "-I" + str(args.prefix / "include"),
             "-L" + str(args.prefix / "lib"),
             "-Wl,-rpath," + str(args.prefix / "lib")]
    compile_result = subprocess.run(
        ["gcc", *flags, str(source), "-lXm", "-lXt", "-lX11", "-lXtst",
         "-o", str(binary)],
        env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, check=False)
    (args.results / "compile.log").write_text(compile_result.stdout)
    if compile_result.returncode:
        raise SystemExit(compile_result.returncode)
    link_result = subprocess.run(["ldd", str(binary)], env=env,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT, text=True,
                                 check=False)
    (args.results / "link.log").write_text(link_result.stdout)
    if link_result.returncode or str(args.prefix / "lib/libXm.so.") not in link_result.stdout:
        raise SystemExit("DropTransfer cancellation probe loaded the wrong libXm")
    for mode in ("immediate", "active", "requestor"):
        command = ["xvfb-run", "-a", "timeout", "10s", str(binary)]
        if mode == "active":
            command.append("active")
        run_result = subprocess.run(
            command, env=env, stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT, text=True, check=False)
        (args.results / ("run-" + mode + ".log")).write_text(
            run_result.stdout)
        print(run_result.stdout, end="")
        if run_result.returncode:
            raise SystemExit(run_result.returncode)
    receipt["success"] = True
finally:
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    receipt["seconds"] = round(time.monotonic() - started, 3)
    (args.results / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
