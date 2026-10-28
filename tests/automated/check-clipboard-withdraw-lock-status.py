#!/usr/bin/env python3
"""Check format withdrawal aborts on every lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardWithdrawFormat(")
end = source.index("int \nXmClipboardCopyByName(", start)
body = source[start:end]

assert "if ( status != ClipboardSuccess )" in body
assert "return status;" in body

print("Clipboard withdrawal aborts on lock failure")
