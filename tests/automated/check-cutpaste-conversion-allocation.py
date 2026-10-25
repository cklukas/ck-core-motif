#!/usr/bin/env python3
"""Check CutPaste converter target and timestamp allocation guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("ClipboardConvertProc(Widget wid")
end = source.index("\nstatic Window", start)
region = source[start:end]

assert "count > INT_MAX - 2" in region
assert "UINT_MAX / sizeof(Atom)" in region
assert "if (ptr == NULL)" in region
assert "if (timestamp == NULL)" in region
assert "rval = False;" in region
print("CutPaste conversion handles target and timestamp allocation failure")
