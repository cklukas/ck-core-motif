#!/usr/bin/env python3
"""Ensure wide-character preedit conversion checks its output buffer."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("size_t capacity = (size_t)insert_length * MB_CUR_MAX + 1;")
end = text.index("\n    } else {", start)
block = text[start:end]

assert "capacity > (size_t)UINT_MAX" in block
assert "mb = XtMalloc((Cardinal)capacity);" in block
assert "if (mb == NULL)" in block
assert "wcstombs(mb" in block

print("text preedit wide conversion allocation guard: ok")
