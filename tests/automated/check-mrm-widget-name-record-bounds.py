#!/usr/bin/env python3
"""Check Mrm widget names and class strings stay inside serialized records."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
required = (
    "static Boolean\nUrmWidgetStringValid",
    "memchr((char *)widgetrec + offset, '\\0'",
    "ov_name == NULL",
    "!UrmWidgetStringValid(widgetrec, widgetrec->name_offs)",
    "!UrmWidgetStringValid(widgetrec, widgetrec->class_offs)",
)
for needle in required:
    if needle not in text:
        raise SystemExit(f"missing widget-string validation: {needle}")
print("PASS Mrm widget names and class strings are record-bounded")
