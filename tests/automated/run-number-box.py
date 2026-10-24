#!/usr/bin/env python3
"""Build and run the installed-prefix NumberBox contract probe on Xvfb."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("prefix", type=Path)
    parser.add_argument("results", type=Path)
    args = parser.parse_args()
    if platform.system() != "Linux" or not args.prefix.is_absolute() or not args.results.is_absolute():
        parser.error("use absolute Linux prefix and results paths")
    if args.results.exists():
        parser.error("results directory must be fresh")
    args.results.mkdir(parents=True)
    (args.results / "tmp").mkdir()
    env = dict(os.environ, TMPDIR=str(args.results / "tmp"),
               LD_LIBRARY_PATH=str(args.prefix / "lib"), LC_ALL="C.UTF-8")
    source = Path(__file__).resolve().parent / "check-number-box.c"
    binary = args.results / "check-number-box"
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
             "-I" + str(args.prefix / "include"), "-L" + str(args.prefix / "lib"),
             "-Wl,-rpath," + str(args.prefix / "lib")]
    receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
               "prefix": str(args.prefix), "architecture": platform.machine(),
               "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
               "steps": []}

    def run(name, command):
        process = subprocess.run(command, env=env, text=True,
                                 stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (args.results / (name + ".log")).write_text(process.stdout)
        receipt["steps"].append({"name": name, "command": command,
                                 "exit": process.returncode})
        print(name, process.returncode, flush=True)
        return process.returncode == 0

    ok = run("compile", ["gcc", *flags, str(source), "-lXm", "-lXt", "-lX11",
                          "-o", str(binary)])
    if ok:
        ok = run("fixture", ["xvfb-run", "-a", str(binary)])
    receipt["success"] = ok
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (args.results / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
