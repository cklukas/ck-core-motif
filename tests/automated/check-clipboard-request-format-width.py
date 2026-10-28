#!/usr/bin/env python3
"""Check clipboard replacement request arithmetic uses unsigned format width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static void \nClipboardReplaceItem(")
end = source.index("static Atom \nClipboardGetAtomFromId(", start)
body = source[start:end]

assert "(unsigned long)format;" in body

print("Clipboard request format arithmetic preserves unsigned width")
