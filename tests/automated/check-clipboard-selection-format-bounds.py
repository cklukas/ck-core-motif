#!/usr/bin/env python3
"""Check selection format values are validated before legacy narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nClipboardGetSelection(")
end = source.index("static void\nClipboardReceiveData(", start)
body = source[start:end]

assert "info.format != 8 && info.format != 16 && info.format != 32" in body
assert "*format = (int)info.format;" in body

print("Clipboard selection format is validated before narrowing")
