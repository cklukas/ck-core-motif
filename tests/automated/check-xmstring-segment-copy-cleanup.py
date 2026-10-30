#!/usr/bin/env python3
"""Check XmString segment-copy failures release owned intermediate buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("_XmStringGetSegment(")
end = source.index("Boolean \n_XmStringGetNextSegment(", start)
body = source[start:end]
assert body.count("XtFree((char *)*tag);") >= 4
assert body.count("XtFree((char *)*text);") >= 3
assert "XmStringTag copied_tag = XtNewString(*tag);" in body
assert "char *tmp = XtMalloc" in body
print("PASS XmString segment-copy failure cleanup")
