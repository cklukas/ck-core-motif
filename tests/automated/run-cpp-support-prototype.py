#!/usr/bin/env python3
"""Run the consumer-only WP-08 C++ support prototype against an install."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import sys


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
parser.add_argument("--cxx", default="c++")
parser.add_argument("--sanitize", action="store_true",
                    help="compile and run with AddressSanitizer and UBSan")
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("the installed prototype currently requires Linux/X11")
if not args.prefix.is_absolute() or not args.out.is_absolute():
    parser.error("--prefix and --out must be absolute")
prefix = args.prefix.resolve(strict=True)
installed_header = prefix / "include/ckmotif++/motif_support.hpp"
installed_pc = prefix / "lib/pkgconfig/ckmotif++-8.pc"
if not installed_header.is_file() or not installed_pc.is_file():
    parser.error("prefix does not contain the installed ckmotif++ header and metadata")
output = args.out.resolve()
if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
    parser.error("--out must be fresh and separate from --prefix")
output.mkdir(parents=True)
(output / "tmp").mkdir()
root = Path(__file__).resolve().parents[2]
source = root / "tests/cpp/support_lifecycle.cpp"
header = root / "cpp/ckmotif++/motif_support.hpp"
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"), PYTHONDONTWRITEBYTECODE="1")
binary = output / "support-lifecycle"
receipt = {
    "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "prefix": str(prefix), "output": str(output),
    "architecture": platform.machine(),
    "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
    "header_sha256": hashlib.sha256(header.read_bytes()).hexdigest(),
    "sanitized": args.sanitize, "steps": [], "success": False,
}


def run(name, command):
    process = subprocess.run(command, cwd=output, env=env, text=True,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    log = output / (name + ".log")
    log.write_text(process.stdout)
    receipt["steps"].append({"name": name, "command": command,
                             "exit": process.returncode, "log": str(log)})
    if process.returncode:
        print(process.stdout, end="")
        raise SystemExit(process.returncode)
    return process.stdout


try:
    flags = ["-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
             "-O2", "-g", "-I" + str(prefix / "include"),
             "-L" + str(prefix / "lib"),
             "-Wl,-rpath," + str(prefix / "lib")]
    if args.sanitize:
        flags.extend(["-O1", "-fsanitize=address,undefined",
                      "-fno-omit-frame-pointer"])
    run("compile", [args.cxx, *flags, str(source), "-lXm", "-lXt", "-lX11",
                     "-o", str(binary)])
    linked = run("link", ["ldd", str(binary)])
    if str(prefix / "lib/libXm.so.8") not in linked:
        raise RuntimeError("prototype did not resolve libXm from selected prefix")
    fixture = ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary)]
    if args.sanitize:
        fixture = ["env", "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1",
                   "UBSAN_OPTIONS=halt_on_error=1", *fixture]
    output_text = run("fixture", fixture)
    if "PASS C++ support prototype" not in output_text:
        raise RuntimeError("prototype did not report PASS")
    receipt["success"] = True
finally:
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

print("PASS WP-08 C++ support prototype")
