#!/usr/bin/env python3
"""Check selection-loss conversion ownership."""

from pathlib import Path


source = Path("lib/Xm/Transfer.c").read_text(encoding="utf-8")

assert "XtPointer value = NULL;" in source
assert "&type, &value, &size, &fmt);\n  XtFree((char *)value);\n\n  LoseProc" in source
assert "static void\nLoseProc(Widget w, Atom *selection)\n{\n  XtPointer value = NULL;" in source

print("selection-loss cleanup checks passed")
