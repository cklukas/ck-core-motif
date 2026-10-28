#!/usr/bin/env python3
"""Check clipboard event Atom comparisons use representable signed payloads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static void \nClipboardEventHandler(")
end = source.index("static void\nClipboardTimeout(", start)
body = source[start:end]

assert "atoms[XmA_MOTIF_CLIP_DATA_REQUEST] > (Atom)LONG_MAX" in body
assert "(long)atoms[XmA_MOTIF_CLIP_DATA_REQUEST]" in body
assert "(long)atoms[XmA_MOTIF_CLIP_DATA_DELETE]" in body

print("Clipboard event Atom comparisons are signed-safe")
