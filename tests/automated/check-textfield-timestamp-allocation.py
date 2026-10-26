#!/usr/bin/env python3
"""Check TextField timestamp selection exports handle allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("} else if (*target == atoms[XmATIMESTAMP])")
end = source.index("} else if (*target == XA_STRING)", start)
branch = source[start:end]

assert "timestamp = (Time *) XtMalloc(sizeof(Time));" in branch
assert "if (timestamp == NULL)" in branch
assert "return False;" in branch

print("TextField timestamp export checks allocation failure")
