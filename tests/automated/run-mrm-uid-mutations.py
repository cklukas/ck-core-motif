#!/usr/bin/env python3
"""Compile a demonstration interface with the UIL compiler, damage the UID file
in a reproducible way and load every copy with the resource manager.

    run-mrm-uid-mutations.py PREFIX RESULTS [--copies N] [--first K] [--uil FILE]

PREFIX is an installed toolkit built with --enable-sanitizers (it provides bin/uil
and libMrm); RESULTS is a fresh absolute directory.  The copies are numbered
from --first; a failure names its copy number, which reproduces it.  By default
the interface is demos/programs/periodic/periodic.uil of this checkout.
"""
import argparse
import os
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
parser.add_argument("--copies", type=int, default=120)
parser.add_argument("--first", type=int, default=0)
parser.add_argument("--chunk", type=int, default=15)
parser.add_argument("--uil", type=Path)
args = parser.parse_args()
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
lib = args.prefix / "lib"
here = Path(__file__).resolve().parent
uil_source = (args.uil or here.parents[1] / "demos/programs/periodic/periodic.uil").resolve()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(lib),
           ASAN_OPTIONS="detect_leaks=0:strict_memcmp=0:abort_on_error=0",
           UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
(args.results / "logs").mkdir()


def run(command, **kw):
    return subprocess.run(command, env=env, stdout=subprocess.PIPE,
                          stderr=subprocess.STDOUT, text=True, check=False, **kw)


def prepare(uil_source, tag):
    """Compile an interface; list its objects and exported values."""
    uid = args.results / (tag + ".uid")
    result = run([str(args.prefix / "bin" / "uil"), "-I.", "-o", str(uid), uil_source.name],
                 cwd=uil_source.parent)
    (args.results / (tag + "-uil.log")).write_text(result.stdout)
    if not uid.exists():
        print(result.stdout, end="")
        raise SystemExit("the UIL compiler produced no UID file for " + str(uil_source))
    names = []
    objects = 0
    for line in uil_source.read_text(errors="replace").splitlines():
        m = re.match(r"\s*object\s+([A-Za-z0-9_]+)", line)
        if m:
            objects += 1
            if objects <= 25:
                names.append("o:" + m.group(1))
            continue
        m = re.match(r"\s*([A-Za-z0-9_]+)\s*:\s*exported", line)
        if m:
            names.append("v:" + m.group(1))
    names_file = args.results / (tag + "-names.txt")
    names_file.write_text("\n".join(names) + "\n")
    return uid, names_file


sources = [("corpus", here / "mrm-uid-corpus.uil"), ("periodic", uil_source)]
prepared = [(tag, *prepare(path, tag)) for tag, path in sources]

binary = args.results / "check-mrm-uid-mutations"
flags = ["-std=gnu17", "-g", "-O1", "-fno-omit-frame-pointer",
         "-fsanitize=address,undefined", "-fno-sanitize-recover=undefined",
         "-Wall", "-Wextra", "-Wno-unused-parameter",
         "-I" + str(args.prefix / "include"), "-L" + str(lib),
         "-Wl,-rpath," + str(lib)]
result = run(["gcc", *flags, str(here / "check-mrm-uid-mutations.c"),
              "-lMrm", "-lXm", "-lXt", "-lX11", "-o", str(binary)])
(args.results / "compile.log").write_text(result.stdout)
if result.returncode:
    print(result.stdout, end="")
    raise SystemExit(result.returncode)

status = 0
for tag, uid, names_file in prepared:
    first = args.first
    end = args.first + args.copies
    stuck = 0
    while first < end:
        count = min(args.chunk, end - first)
        result = run(["xvfb-run", "-a", str(binary), str(uid), str(names_file),
                      str(args.results / "work.uid"), str(first), str(count),
                      str(args.results / "logs")])
        with (args.results / "run.log").open("a") as log:
            log.write(tag + ": " + result.stdout)
        for line in result.stdout.splitlines():
            if (line.startswith("copy ") and "failed" in line) or "FAIL" in line or \
               line.startswith(("note:", "unchanged", "server lost")):
                print(tag + ": " + line)
        if result.returncode == 77:
            # the X server went away on one copy: continue after it
            m = re.findall(r"server lost at copy (\d+)", result.stdout)
            lost = int(m[-1]) if m else None
            if lost is None or lost < first:
                stuck += 1
                if stuck > 2:
                    raise SystemExit("no X server could be used")
                continue
            first = lost + 1
            continue
        if result.returncode:
            status = 1
        first += count
print(f"mrm uid mutations: {args.copies} copies of each of {len(prepared)} interfaces "
      f"{'FAILED' if status else 'ok'}")
raise SystemExit(status)
