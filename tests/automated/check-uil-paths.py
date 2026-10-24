#!/usr/bin/env python3
"""Verify that an include path exceeding the legacy FCB field fails safely."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("uil", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
if not args.uil.is_absolute() or not args.results.is_absolute():
    parser.error("uil and results must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
long_dir = (args.results / ("include-" + "x" * 100) /
            ("nested-" + "y" * 100) /
            ("leaf-" + "z" * 100))
long_dir.mkdir(parents=True)
(long_dir / "included.msg").write_text("\n")
source = args.results / "include-path.uil"
source.write_text(
    "module include_path_regression\n"
    "version = 'v1.0'\n"
    "names = case_sensitive\n"
    'include file "included.msg";\n'
    "end module;\n"
)
env = dict(os.environ, LC_ALL="C.UTF-8", TMPDIR=str(args.results / "tmp"))
result = subprocess.run(
    [str(args.uil), "-I" + str(long_dir), "-m", source],
    cwd=args.results, env=env, capture_output=True, timeout=60,
)
(args.results / "uil.log").write_bytes(result.stdout + result.stderr)
assert result.returncode != 0, "overlong include path was accepted"
assert b"AddressSanitizer" not in result.stdout + result.stderr
print("PASS UIL rejects overlong include paths without memory corruption")
