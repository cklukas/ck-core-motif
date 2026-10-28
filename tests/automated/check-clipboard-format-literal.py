#!/usr/bin/env python3
"""Check clipboard format-atom construction uses a literal format string."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static Atom \nClipboardGetAtomFromFormat(")
end = source.index("static int \nClipboardGetLenFromFormat(", start)
body = source[start:end]

assert '"_MOTIF_CLIP_FORMAT_%s", format_name' in body
assert "char *atomname_format" not in body

print("Clipboard format-atom construction uses a literal format")
