#!/usr/bin/env python3
"""Check I18List target-list conversion allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
start = source.index("if (cs->target == atoms[XmATARGETS])")
end = source.index("\n    else if (cs->target == atoms[XmACOMPOUND_TEXT]", start)
region = source[start:end]

assert "if (targs == NULL)" in region
assert "cs->status = XmCONVERT_REFUSE;" in region
assert "XtMalloc(sizeof(Atom) * 6)" in region
assert "if (XA_STRING != C_ENCODING)" in region
assert "targs[n++] = XA_PIXMAP;" in region
print("I18List target-list conversion handles allocation and capacity")
