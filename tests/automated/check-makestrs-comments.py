#!/usr/bin/env python3
"""Check source-identity comment ownership and boundary handling in makestrs."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("binary", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
assert args.binary.is_absolute() and args.results.is_absolute()
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"))
receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
           "binary": str(args.binary),
           "binary_sha256": hashlib.sha256(args.binary.read_bytes()).hexdigest(),
           "cases": [], "success": False}
start = time.monotonic()
cases = [("empty", b"! $$\n", None),
         ("single", b"! $A$\n", b"A"),
         ("unspaced", b"! $AB$\n", b"AB"),
         ("spaced", b"! $AB $\n", b"AB"),
         ("whitespace", b"! $AB \t$\n", b"AB"),
         ("later-valid", b"! $$\n! $REAL $\n", b"REAL"),
         ("unpaired", b"! $AB\n", None)]
try:
    for name, data, expected in cases:
        run = subprocess.run([str(args.binary)], input=data, cwd=args.results,
                             env=env, capture_output=True, timeout=10)
        (args.results / (name + ".out")).write_bytes(run.stdout)
        (args.results / (name + ".err")).write_bytes(run.stderr)
        assert run.returncode == 0, (name, run.stderr.decode(errors="replace"))
        assert not run.stderr, (name, run.stderr)
        if expected is not None:
            assert b"/* " + expected + b" */\n" in run.stdout, name
        else:
            assert b"/*  */" not in run.stdout, name
        receipt["cases"].append({"name": name, "input_hex": data.hex(),
                                 "output_sha256": hashlib.sha256(run.stdout).hexdigest()})
    receipt["success"] = True
finally:
    receipt.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                   seconds=time.monotonic()-start)
    (args.results / "receipt.json").write_text(json.dumps(receipt, indent=2)+"\n")
print("makestrs comment boundaries and ownership passed")
