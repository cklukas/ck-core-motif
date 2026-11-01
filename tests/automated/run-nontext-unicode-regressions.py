#!/usr/bin/env python3
"""Compile and run the non-text Unicode widget regression on Xvfb."""
import argparse
import hashlib
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"),
           LC_ALL="C.UTF-8", LD_LIBRARY_PATH=str(args.prefix / "lib"))
source = Path(__file__).resolve().parent / "check-nontext-unicode.c"
binary = args.results / "check-nontext-unicode"
flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
         "-I" + str(args.prefix / "include"), "-L" + str(args.prefix / "lib"),
         "-Wl,-rpath," + str(args.prefix / "lib")]
compile_result = subprocess.run(
    ["gcc", *flags, str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    check=False)
(args.results / "compile.log").write_text(compile_result.stdout)
if compile_result.returncode:
    raise SystemExit(compile_result.returncode)
ldd_result = subprocess.run(["ldd", str(binary)], env=env,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, text=True,
                            check=False)
(args.results / "ldd.log").write_text(ldd_result.stdout)
loaded_libxm = any(
    line.lstrip().startswith("libXm.so") and str(args.prefix / "lib") in line
    for line in ldd_result.stdout.splitlines()
)
if ldd_result.returncode or not loaded_libxm:
    raise SystemExit("the regression binary did not load libXm from the requested prefix")
library = next((candidate.resolve() for candidate in
                sorted((args.prefix / "lib").glob("libXm.so.*"))
                if candidate.is_file()), None)
if library is None:
    raise SystemExit("the requested prefix has no libXm shared library")
receipt = "\n".join((
    "source_sha256=" + sha256_file(source),
    "binary_sha256=" + sha256_file(binary),
    "libxm=" + str(library),
    "libxm_sha256=" + sha256_file(library),
    "",
))
(args.results / "receipt.txt").write_text(receipt)
run_result = subprocess.run(["xvfb-run", "-a", str(binary)], env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, check=False)
(args.results / "run.log").write_text(run_result.stdout)
if run_result.returncode:
    raise SystemExit(run_result.returncode)
print(run_result.stdout, end="")
