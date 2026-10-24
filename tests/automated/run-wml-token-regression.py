#!/usr/bin/env python3
"""Exercise wmluiltok's bounded synonym handling."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("wmluiltok", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
if not args.wmluiltok.is_absolute() or not args.results.is_absolute():
    parser.error("wmluiltok and results must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
env = dict(os.environ, TMPDIR=str(args.results / "tmp"))
(args.results / "tmp").mkdir()
source = "".join(
    f"%token token{i} {i + 1} class{i}\n" for i in range(1001)
)
result = subprocess.run(
    [str(args.wmluiltok)], input=source, env=env, text=True,
    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=False,
)
(args.results / "run.log").write_text(result.stdout)
assert result.returncode != 0, "oversized synonym list was accepted"
assert "too many tokens" in result.stdout
print("PASS wmluiltok rejects oversized token tables")
