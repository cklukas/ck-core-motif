#!/usr/bin/env python3
"""Check XmString segment-copy failures release owned intermediate buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("_XmStringGetSegment(")
end = source.index("Boolean \n_XmStringGetNextSegment(", start)
body = source[start:end]
# *tag and *text point into the string until they are copied: only the copies are released
assert "XtFree((char *)*tag);" not in body
assert "XtFree((char *)*text);" not in body
assert body.count("XtFree(copied_tag);") == 1
assert body.count("XtFree(copied_text);") == 1
assert "copied_tag = XtNewString(*tag);" in body
assert "copied_text = XtMalloc" in body
print("PASS XmString segment-copy failure cleanup")
