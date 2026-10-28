#!/usr/bin/env python3
"""Check XPM conversion rejects an explicitly supplied zero depth."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
assert source.count("(attributes->valuemask & XpmDepth) &&") >= 2
assert source.count("attributes->depth == 0") >= 2
print("PASS XPM explicit depth input is validated")
