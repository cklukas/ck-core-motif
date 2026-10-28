#!/usr/bin/env python3
"""Check named clipboard conversion results are not narrowed to short."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static Boolean \nClipboardGetByNameItem(")
end = source.index("static Boolean\nClipboardConvertProc(", start)
body = source[start:end]

assert "int dataok;" in body
assert "short dataok;" not in body
assert "dataok = ClipboardRequestDataAndWait" in body

print("Clipboard named-conversion result preserves width")
