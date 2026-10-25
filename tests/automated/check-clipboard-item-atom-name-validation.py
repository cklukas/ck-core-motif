#!/usr/bin/env python3
"""Check that clipboard item atom names are built with bounded input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static Atom \nClipboardGetAtomFromId(")
end = source.index("static Atom \nClipboardGetAtomFromFormat(", start)
region = source[start:end]

assert "if (itemid < 0)" in region
assert "snprintf(atomname, sizeof(atomname)" in region
assert "written < 0" in region
assert "(size_t)written >= sizeof(atomname)" in region
print("PASS clipboard item atom names are bounded")
