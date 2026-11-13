#!/usr/bin/env python3
"""Compile check-mrm-value-bounds.c against the production lib/Mrm sources
under ASan/UBSan and run its cases.

    run-mrm-value-bounds.py RESULTS [--source-revision REV]

RESULTS is a fresh absolute directory.  Without a revision the working tree is
tested and every case must pass.  With --source-revision REV (for example HEAD)
the library sources of that revision are tested instead: the cases that mark a
defect must fail there and the runner exits 0 only then.  See sanitized_sources.py.
"""
from pathlib import Path
import sys

from sanitized_sources import Session

# (case, defect): a defect case fails on a library that has the defect
CASES = [
    ("short-stream", True),
    ("truncated-header", True),
    ("short-stream-fetch-literal", True),
    ("valid-stream", False),
    ("colour-name-not-terminated", True),
    ("colour-rgb-short", True),
    ("valid-colour", False),
]

session = Session(__doc__)
binary = session.build_mrm(Path(__file__).resolve().parent / "check-mrm-value-bounds.c")
sys.exit(session.finish(session.run(binary, CASES), "mrm value bounds"))
