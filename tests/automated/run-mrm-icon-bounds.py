#!/usr/bin/env python3
"""Compile check-mrm-icon-bounds.c against the production lib/Mrm sources under
ASan/UBSan and run its cases.

    run-mrm-icon-bounds.py RESULTS [--source-revision REV]

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
    ("pixel-beyond-colour-table", True),
    ("last-pixel-beyond-colour-table", True),
    ("unknown-pixel-size", True),
    ("missing-colour-table", True),
    ("colour-table-too-small", True),
    ("valid-four-bit-icon", False),
    ("valid-one-bit-icon", False),
    ("valid-full-eight-bit-icon", False),
]

session = Session(__doc__)
binary = session.build_mrm(Path(__file__).resolve().parent / "check-mrm-icon-bounds.c")
sys.exit(session.finish(session.run(binary, CASES), "mrm icon bounds"))
