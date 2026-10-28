#!/usr/bin/env python3
"""Check convert-time format records are released after callback retrieval."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static Boolean \nClipboardGetByNameItem(")
end = source.index("static Boolean\nClipboardConvertProc(", start)
body = source[start:end]

assert "ClipboardRequestDataAndWait" in body
request = body.index("ClipboardRequestDataAndWait")
release = body.index("XtFree( (char *)matchformat );", request)
assert release < body.index("return(dataok != 0);", release)

print("Clipboard conversion releases the format record")
