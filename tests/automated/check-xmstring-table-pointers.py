#!/usr/bin/env python3
"""Check that XmString table matching validates component pointers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
start = text.index("XmStringToXmStringTable(")
end = text.index("\n}\n", start)
region = text[start:end]

required = "val != NULL && b_val != NULL && memcmp(val, b_val, len) == 0"
if region.count(required) != 2:
    raise SystemExit("XmString table component pointer guards missing")

print("XmString table matching validates component pointers")
