#!/usr/bin/env python3
"""Check safe RowActions XmString copying."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text(encoding="utf-8")
start = source.index("static XmString string_copy")
end = source.index("static char *button_text", start)
region = source[start:end]

assert "if (!text) return NULL;" in region
assert "XmStringCreateLocalized(text)" in region
print("PASS RowActions string copies validate conversion allocation")
