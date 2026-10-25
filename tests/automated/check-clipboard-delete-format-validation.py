#!/usr/bin/env python3
"""Check that delete-format validates both serialized clipboard records."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static void \nClipboardDeleteFormat(")
end = source.index("static void \nClipboardDeleteFormats(", start)
region = source[start:end]

assert "status = ClipboardFindItem" in region
assert "formatlength < sizeof(ClipboardFormatItemRec)" in region
assert "length < sizeof(ClipboardDataItemRec)" in region
assert "XtFree( (char *) formatitem );" in region
print("PASS clipboard delete-format records are validated")
