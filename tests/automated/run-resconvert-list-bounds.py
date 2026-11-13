#!/usr/bin/env python3
"""Compile check-resconvert-list-bounds.c against the production lib/Xm sources
under ASan/UBSan and run its cases.

    run-resconvert-list-bounds.py RESULTS [--source-revision REV]

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
    ("tab-unit-converter", True),
    ("token-leading-delimiter", True),
    ("token-leading-white-space-delimiter", True),
    ("atom-list-leading-comma", True),
    ("token-escape-at-end", True),
    ("token-single-characters", True),
    ("token-empty-and-white-space", True),
    ("token-escaped-delimiter", True),
    ("valid-tab-list", False),
    ("valid-token-multiple-characters", False),
]

session = Session(__doc__)
binary = session.build_xm(Path(__file__).resolve().parent / "check-resconvert-list-bounds.c", unity=True)
sys.exit(session.finish(session.run(binary, CASES), "resource converter list bounds"))
