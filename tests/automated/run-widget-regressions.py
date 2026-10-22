#!/usr/bin/env python3
"""Compile installed-library consumers and run widget API regressions on Linux."""
import argparse
import datetime
from decimal import Decimal
import hashlib
import json
import os
from pathlib import Path
import platform
import random
import signal
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
parser.add_argument("--cc", choices=("gcc", "clang"), default="gcc")
parser.add_argument("--sanitize", action="store_true",
                    help="Instrument consumers; instrument libXm separately for full coverage")
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("run in an isolated Linux task workspace")
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
for name in ("tmp", "cache", "config", "locales"):
    (args.results / name).mkdir()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"),
           XDG_CACHE_HOME=str(args.results / "cache"),
           XDG_CONFIG_HOME=str(args.results / "config"),
           LOCPATH=str(args.results / "locales"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(args.prefix / "lib"))
sources = Path(__file__).resolve().parent
receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
           "compiler": args.cc, "prefix": str(args.prefix),
           "architecture": platform.machine(), "steps": [], "sources": {},
           "consumer_sanitizers": args.sanitize,
           "scope": "Installed-library API fixtures on Xvfb; not application or live XIM acceptance"}
started = time.monotonic()


def run(label, command):
    begin = time.monotonic()
    process = subprocess.Popen([str(x) for x in command], env=env,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                               start_new_session=True)
    try:
        output, _ = process.communicate(timeout=120)
        code = process.returncode
    except subprocess.TimeoutExpired:
        os.killpg(process.pid, signal.SIGTERM)
        try:
            output, _ = process.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            output, _ = process.communicate()
        output += b"\nTIMEOUT: terminated this fixture's process group\n"
        code = 124
    (args.results / (label + ".log")).write_bytes(output)
    receipt["steps"].append({"name": label, "command": [str(x) for x in command],
                             "exit": code,
                             "seconds": round(time.monotonic() - begin, 3)})
    print(label, code, flush=True)
    return code == 0


try:
    libraries = [p for p in (args.prefix / "lib").glob("libXm.so.*") if not p.is_symlink()]
    if len(libraries) != 1:
        raise RuntimeError("prefix must contain exactly one actual libXm ELF")
    receipt["libraries"] = {str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in libraries}
    ok = run("locale", ["localedef", "--no-archive", "-i", "de_DE", "-f", "UTF-8",
                         args.results / "locales/de_DE.UTF-8"])
    # Independent decimal arithmetic oracle through the public widget API.
    randomizer = random.Random(20261023)
    corpus = []
    for _ in range(2000):
        digits = randomizer.randint(-999999999999999999, 999999999999999999)
        exponent = randomizer.randint(-60, 60)
        decimals = randomizer.randint(0, 25)
        text = str(digits) + "e" + str(exponent)
        value = int(Decimal(text).scaleb(decimals))
        kind = -1 if value < -2147483648 else 1 if value > 2147483647 else 0
        value = max(-2147483648, min(2147483647, value))
        corpus.append(f"{text} {decimals} {kind} {value}\n")
    corpus_file = args.results / "decimal-oracle.txt"
    corpus_file.write_text("".join(corpus))
    receipt["decimal_oracle_sha256"] = hashlib.sha256(corpus_file.read_bytes()).hexdigest()
    flags = ["-Wall", "-Wextra", "-Werror", "-g", "-O2",
             "-I" + str(args.prefix / "include"), "-L" + str(args.prefix / "lib"),
             "-Wl,-rpath," + str(args.prefix / "lib")]
    if args.sanitize:
        flags += ["-fsanitize=address,undefined,float-cast-overflow", "-fno-omit-frame-pointer"]
    for name in ("spinbox", "dpi", "list-unicode"):
        source = sources / ("check-" + name + ".c")
        receipt["sources"][source.name] = hashlib.sha256(source.read_bytes()).hexdigest()
        binary = args.results / ("check-" + name)
        if not run("compile-" + name, [args.cc, *flags, source,
                                       "-lXm", "-lXt", "-lX11", "-o", binary]):
            ok = False
            continue
        if not run("link-" + name, ["ldd", binary]):
            ok = False
        link = (args.results / ("link-" + name + ".log")).read_text()
        if str(args.prefix / "lib/libXm.so.") not in link:
            raise RuntimeError("consumer loaded the wrong libXm")
        modes = ("format", "increment", "validate", "items", "delete-bound", "index",
                 "step", "timer", "locale", "matrix") if name == "spinbox" else (None,)
        for mode in modes:
            label = name + ("-" + mode if mode else "")
            command = ["xvfb-run", "-a", binary] + ([mode] if mode else [])
            if mode == "matrix":
                command.append(corpus_file)
            if not run(label, command):
                ok = False
    receipt["success"] = ok
finally:
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    receipt["seconds"] = round(time.monotonic() - started, 3)
    (args.results / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
raise SystemExit(0 if ok else 1)
