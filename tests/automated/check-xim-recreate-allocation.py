#!/usr/bin/env python3
"""Check that externally-created XIC reconstruction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("recreate_xic_info(XIC")
end = source.index("/* Attempt to create an XmImXICInfo struct.", start)
region = source[start:end]
assert "xic_info = XtNew(XmImXICRec);" in region
assert "if (xic_info == NULL)" in region
assert "return NULL;" in region
print("PASS external XIC reconstruction allocation is guarded")
