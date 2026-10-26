#!/usr/bin/env python3
"""Check that wide XIM preedit conversion has multibyte capacity."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

for required in (
    "size_t mb_capacity, converted;",
    "(mb_capacity = (size_t)insert_length * MB_CUR_MAX + 1)",
    "converted = wcstombs(mb,",
    "if (converted == (size_t)-1)",
):
    if required not in region:
        raise SystemExit(f"missing multibyte preedit safeguard: {required}")
print("TextField wide preedit conversion preserves multibyte capacity")
