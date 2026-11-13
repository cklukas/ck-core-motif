#!/usr/bin/env python3
"""Compile check-xmstring-stream-bounds.c against the production lib/Xm sources
under ASan/UBSan and run its cases.

    run-xmstring-stream-bounds.py RESULTS [--source-revision REV]

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
    ("nonopt-length", True),
    ("nonopt-missing-length", True),
    ("nonopt-truncated-long-length", True),
    ("direction-without-value", True),
    ("push-without-value", True),
    ("cache-embedded-nul", True),
    ("next-segment-borrowed-tag", True),
    ("next-segment-oversized-text", True),
    ("next-segment-wide-text-odd-length", True),
    ("next-segment-wide-text", True),
    ("valid-single-segment", False),
    ("valid-unoptimizable", False),
    ("valid-segments", False),
    ("valid-cache-tags", False),
]

session = Session(__doc__)
binary = session.build_xm(Path(__file__).resolve().parent / "check-xmstring-stream-bounds.c")
sys.exit(session.finish(session.run(binary, CASES), "xmstring stream bounds"))
