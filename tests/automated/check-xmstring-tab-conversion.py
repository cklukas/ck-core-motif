#!/usr/bin/env python3
"""Check XmString tab conversion guards missing display/screen state."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("TabVal(Display *d,")
end = source.index("\n}\n", start)
region = source[start:end]

assert "d == NULL || pscreen == NULL" in region
assert "(*pscreen == NULL && w == 0)" in region
assert "return 0;" in region
assert "assert(w || *pscreen);" not in region

print("XmString tab conversion guards missing screen state")
