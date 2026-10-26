#!/usr/bin/env python3
"""Check that clipboard MOVE cleanup cannot free an uninitialized value."""

from pathlib import Path


source = Path("lib/Xm/Transfer.c").read_text(encoding="utf-8")

assert "XtPointer value = NULL;" in source
assert "if (op == XmMOVE && transferred != 0) {\n    value = NULL;" in source

print("clipboard MOVE cleanup checks passed")
