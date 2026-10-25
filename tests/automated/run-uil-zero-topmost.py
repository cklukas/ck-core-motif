#!/usr/bin/env python3
"""Verify UIL modules containing only exported literals can be written."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("uil", type=Path)
parser.add_argument("module", type=Path)
parser.add_argument("output", type=Path)
args = parser.parse_args()
for path in (args.uil, args.module, args.output):
    if not path.is_absolute():
        parser.error("uil, module and output must be absolute")
if args.output.exists():
    parser.error("output must not already exist")
args.output.parent.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, TMPDIR=str(args.output.parent / "tmp"))
(args.output.parent / "tmp").mkdir()
result = subprocess.run(
    [str(args.uil), "-o", str(args.output), str(args.module),
     "-I" + str(args.module.parent), "-I" + str(args.uil.parent)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
    text=True, check=False,
)
(args.output.parent / "run.log").write_text(result.stdout)
assert result.returncode == 0, result.stdout
assert args.output.is_file() and args.output.stat().st_size > 0
print("PASS UIL zero-topmost module generation")
