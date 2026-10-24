#!/usr/bin/env python3
"""Compile and run the installed-library Form/layout diagnostic corpus."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import time


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("WP-10 currently requires Linux/Xvfb")
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
source = Path(__file__).resolve().parent / "check-form-layout-corpus.c"
binary = output / "check-form-layout-corpus"
result_file = output / "form-layout-corpus.txt"
receipt = {"source": str(source), "source_sha256": sha256(source),
           "prefix": str(prefix), "output": str(output), "steps": [],
           "success": False}
started = time.monotonic()


def run(name, command):
    command = [str(item) for item in command]
    log = output / (name + ".log")
    began = time.monotonic()
    completed = subprocess.run(command, cwd=output, env=env, text=True,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                               check=False)
    log.write_text(completed.stdout)
    receipt["steps"].append({"name": name, "command": command,
                             "exit": completed.returncode,
                             "seconds": round(time.monotonic() - began, 3),
                             "log": str(log)})
    if completed.returncode:
        print(completed.stdout, end="", flush=True)
        raise SystemExit(completed.returncode)
    return completed.stdout


try:
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
             "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
             "-Wl,-rpath," + str(prefix / "lib")]
    run("compile", ["cc", *flags, source, "-lXm", "-lXt", "-lX11", "-ldl",
                     "-o", binary])
    linked = run("link", ["ldd", binary])
    if str(prefix / "lib/libXm.so.8") not in linked:
        raise RuntimeError("corpus did not resolve libXm from selected prefix")
    log = run("run", ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24",
                       binary, result_file])
    if "PASS Form layout diagnostic corpus" not in log:
        raise RuntimeError("corpus did not report PASS")
    lines = result_file.read_text().splitlines()
    if not lines or lines[0] != "SCHEMA 1":
        raise RuntimeError("corpus output has no recognized schema")
    metrics = {line.split(" ", 1)[0]: int(line.split(" ", 1)[1])
               for line in lines[1:] if " " in line}
    for name in ("GEOMETRY_REQUESTS", "GEOMETRY_QUERIES", "EVENT_CONFIGURE",
                 "EVENT_EXPOSE", "EVENT_MAP", "EVENT_UNMAP",
                 "CALLBACK_MUTATIONS", "NOOP_GEOMETRY_REQUESTS",
                 "NOOP_GEOMETRY_QUERIES", "NOOP_CONFIGURE"):
        if name not in metrics:
            raise RuntimeError(f"missing metric {name}")
    if metrics["GEOMETRY_REQUESTS"] == 0 or metrics["GEOMETRY_QUERIES"] == 0:
        raise RuntimeError("corpus did not observe geometry negotiation")
    if metrics["EVENT_EXPOSE"] == 0 or metrics["EVENT_CONFIGURE"] == 0:
        raise RuntimeError("corpus did not observe visible layout activity")
    if metrics["NOOP_GEOMETRY_REQUESTS"] != 0 or metrics["NOOP_CONFIGURE"] != 0:
        raise RuntimeError("no-op control triggered unexpected geometry work")
    receipt["result_sha256"] = sha256(result_file)
    receipt["metrics"] = metrics
    receipt["success"] = True
finally:
    receipt["ended_utc"] = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    receipt["seconds"] = round(time.monotonic() - started, 3)
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

print("PASS WP-10 Form layout diagnostic corpus")
