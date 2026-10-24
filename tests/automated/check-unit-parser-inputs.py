#!/usr/bin/env python3
"""Check defensive argument handling in the unit-string parser."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResInd.c"
text = source.read_text(encoding="utf-8")

guard = "if (spec == NULL || float_value == NULL || unit_type == NULL)"
if guard not in text:
    raise SystemExit("unit parser null-input guard is missing")
if "return XmPARSE_ERROR;" not in text:
    raise SystemExit("unit parser null input does not return parse failure")

print("unit parser rejects null arguments")
