#!/usr/bin/env python3
"""Check RowActions keeps XtMalloc requests within Cardinal width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text(encoding="utf-8")
start = source.index("static char *button_text")
end = source.index("static void free_items", start)
region = source[start:end]

assert "if (length > UINT_MAX)" in region
assert "XtMalloc((Cardinal)length)" in region
print("PASS RowActions button text allocation is Cardinal-bounded")
