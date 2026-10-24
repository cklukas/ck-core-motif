#!/usr/bin/env python3
"""Check safe action-parameter parsing for representation types."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text(encoding="utf-8")

if "if (parameter == NULL || result == NULL)" not in text:
    raise SystemExit("action parameter null guard is missing")
if "isspace((unsigned char)parameter[i])" not in text:
    raise SystemExit("action parameter whitespace check is not unsigned")
if "isdigit((unsigned char)parameter[i])" not in text:
    raise SystemExit("action parameter digit check is not unsigned")

print("representation-type action parameter parsing is guarded")
