#!/usr/bin/env python3
"""Check Mrm conversion paths handle allocation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
checks = {
    "if (cbptr == NULL)\n\t    return;": "translated callback",
    "if (fontlist == NULL)\n\t\treturn;": "CreateArglist font list",
    "if (fontlist == NULL)\n\t\t    return MrmFAILURE;": "FixupCallback font list",
    "if (cbdesc == NULL)\n    return MrmFAILURE;": "callback descriptor",
    "if (cbptr == NULL)\n    return NULL;": "old callback allocation",
}
for needle, label in checks.items():
    if needle not in text:
        raise SystemExit(f"Mrm {label} allocation guard is missing")

print("Mrm conversion allocation failures are handled")
