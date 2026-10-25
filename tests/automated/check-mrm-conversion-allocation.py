#!/usr/bin/env python3
"""Check Mrm conversion paths handle allocation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
checks = {
    "if (cbptr == NULL)": "translated callback",
    "if (fontlist == NULL)": "font list",
    "if (cbdesc == NULL || bufptr == NULL": "callback descriptor",
    "if (cbptr == NULL)\n    return NULL;": "old callback allocation",
}
for needle, label in checks.items():
    if needle not in text:
        raise SystemExit(f"Mrm {label} allocation/input guard is missing")
assert "if (fontlist == NULL)\n\t\t    return MrmFAILURE;" in text

print("Mrm conversion allocation failures are handled")
