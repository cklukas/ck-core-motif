#!/usr/bin/env python3
"""Check XIM display initialization allocation guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("xim_info = XtNew(XmImDisplayRec);")
end = source.index("\n  XtGetApplicationNameAndClass", start)
region = source[start:end]
assert "if (xim_info == NULL)" in region
assert region.index("if (xim_info == NULL)") < region.index("bzero")
assert "if (tmp != NULL)" in region
assert region.index("if (tmp != NULL)") < region.index("memcpy(tmp")
print("PASS XmIm initialization allocation guards")
