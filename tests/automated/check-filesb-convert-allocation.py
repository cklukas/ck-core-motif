#!/usr/bin/env python3
"""Check FileSB conversion target-array allocation failure is refused."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
start = source.index("targs = (Atom *) XtMalloc(sizeof(Atom) * 2);")
body = source[start:source.index("targs[0]", start)]
assert "if (targs == NULL)" in body
assert "cs -> status = XmCONVERT_REFUSE;" in body
assert "return;" in body
print("PASS FileSB conversion target allocation is guarded")
