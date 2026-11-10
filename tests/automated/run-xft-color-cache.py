#!/usr/bin/env python3
"""Compile check-xft-color-cache.c against an installed prefix and run it
against two virtual X servers, one 24-bit TrueColor and one 8-bit PseudoColor.

    run-xft-color-cache.py PREFIX RESULTS [--old-api]

PREFIX is the installed toolkit (build and run it and this program with the
same sanitizer list); RESULTS is a fresh absolute directory.  --old-api builds
the program for the toolkit that cached colours by pixel alone.
"""
import argparse
import os
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
parser.add_argument("--old-api", action="store_true")
args = parser.parse_args()
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
lib = args.prefix / "lib"
here = Path(__file__).resolve().parent
env = dict(os.environ, TMPDIR=str(args.results / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(lib),
           ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:halt_on_error=1",
           UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
binary = args.results / "check-xft-color-cache"
sanitize = os.environ.get("MOTIF_TEST_SANITIZE", "address,undefined")
flags = ["-std=gnu17", "-Wall", "-Wextra", "-Wno-unused-parameter", "-g", "-O1",
         "-fno-omit-frame-pointer", "-I" + str(args.prefix / "include"),
         "-I/usr/include/freetype2", "-L" + str(lib), "-Wl,-rpath," + str(lib)]
if args.old_api:
    flags.append("-DXFT_CACHE_OLD_API")
if sanitize:
    flags += ["-fsanitize=" + sanitize, "-fno-sanitize-recover=undefined"]
result = subprocess.run(
    ["gcc", *flags, str(here / "check-xft-color-cache.c"),
     "-lXm", "-lXft", "-lXt", "-lX11", "-ldl", "-o", str(binary)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    check=False)
(args.results / "compile.log").write_text(result.stdout)
if result.returncode:
    print(result.stdout, end="")
    raise SystemExit(result.returncode)


def free_display():
    for number in range(70, 120):
        if not Path(f"/tmp/.X11-unix/X{number}").exists() and \
           not Path(f"/tmp/.X{number}-lock").exists():
            yield number


servers = []
try:
    numbers = free_display()
    shown = []
    for depth in (24, 8):
        number = next(numbers)
        proc = subprocess.Popen(
            ["Xvfb", f":{number}", "-screen", "0", f"640x480x{depth}", "-nolisten", "tcp"],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        servers.append(proc)
        for _ in range(100):
            if Path(f"/tmp/.X11-unix/X{number}").exists():
                break
            time.sleep(0.1)
        shown.append(f":{number}")
    env["DISPLAY"], env["DISPLAY2"] = shown
    time.sleep(0.5)
    result = subprocess.run([str(binary)], env=env, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, text=True, check=False)
finally:
    for proc in servers:
        proc.terminate()
    for proc in servers:
        proc.wait()
(args.results / "run.log").write_text(result.stdout)
print(result.stdout, end="")
raise SystemExit(result.returncode)
