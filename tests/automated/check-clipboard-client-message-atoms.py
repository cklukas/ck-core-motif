#!/usr/bin/env python3
"""Check clipboard client-message Atom payloads and message types."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nClipboardSendMessage(")
end = source.index("static int \nClipboardDataIsReady(", start)
body = source[start:end]

assert "atoms[XmA_MOTIF_CLIP_DATA_REQUEST] > (Atom)LONG_MAX" in body
assert "atoms[XmA_MOTIF_CLIP_DATA_DELETE] > (Atom)LONG_MAX" in body
assert "default:\n\t    return 0;" in body

print("Clipboard client-message Atom payloads are bounded")
