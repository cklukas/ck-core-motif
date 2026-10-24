#!/usr/bin/env python3
"""Check safe empty/null handling for color-name digit suffix detection."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c"
text = source.read_text(encoding="utf-8")

if "if (str == NULL || *str == '\\0')" not in text:
    raise SystemExit("color digit-suffix empty/null guard is missing")
if "isdigit((unsigned char)*c)" not in text:
    raise SystemExit("color digit-suffix digit check is not unsigned")
if "c >= str &&" in text:
    raise SystemExit("color digit-suffix check still decrements before empty guard")

print("color digit-suffix detection handles empty and null names")
