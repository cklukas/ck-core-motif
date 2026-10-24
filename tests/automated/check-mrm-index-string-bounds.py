#!/usr/bin/env python3
"""Check Mrm index strings are bounded by remaining record storage."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c"
text = source.read_text(encoding="utf-8")
if text.count("Idb__INX_StringTerminated") < 4:
    raise SystemExit("Mrm index string checks are not centralized across search paths")
if "available = capacity - offset" not in text:
    raise SystemExit("Mrm index string helper does not derive remaining capacity")
if "strnlen (stgbase +" in text:
    raise SystemExit("Mrm index traversal still uses an unbounded record-relative strnlen")
print("Mrm index strings are checked against remaining record capacity")
