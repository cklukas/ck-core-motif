#!/usr/bin/env python3
"""Check that clipboard property replacement validates X11 inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static void \nClipboardReplaceItem(")
end = source.index("static Atom \nClipboardGetAtomFromId(", start)
region = source[start:end]

assert "format != 8 && format != 16 && format != 32" in region
assert "pointer == NULL && length != 0" in region
assert "length % BYTELENGTH(1, format)" in region
assert "return;" in region
print("PASS clipboard property replacement validates inputs")
