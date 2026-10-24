#!/usr/bin/env python3
"""Check null handling at the public XmeParseUnits API boundary."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResInd.c"
text = source.read_text(encoding="utf-8")

needle = "if (spec == NULL || unitType == NULL)\n    return XmPARSE_ERROR;"
if needle not in text:
    raise SystemExit("XmeParseUnits null-input guard is missing")

print("XmeParseUnits rejects null arguments")
