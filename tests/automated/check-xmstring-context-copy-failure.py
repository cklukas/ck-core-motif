#!/usr/bin/env python3
"""Check context-copy allocation failures invalidate the target context."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("void\n_XmStringContextCopy(")
end = source.index("void\n_XmStringContextFree(", start)
body = source[start:end]
assert body.count("_XmStrContError(target) = True;") >= 3
assert "_XmStrContRendCount(target) = 0;" in body
assert "_XmStrContRendTags(target) = NULL;" in body
print("PASS XmString context-copy failure state")
