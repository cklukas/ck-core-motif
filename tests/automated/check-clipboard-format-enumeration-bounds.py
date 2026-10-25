#!/usr/bin/env python3
"""Check that clipboard format enumeration validates serialized bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static ClipboardFormatItem \nClipboardFindFormat(")
end = source.index("static void \nClipboardDeleteFormat(", start)
region = source[start:end]

assert "header == NULL" in region
assert "reclength < sizeof(ClipboardDataItemRec)" in region
assert "formatIdList > ULONG_MAX / CONVERT_32_FACTOR" in region
assert "formatCount" in region and "sizeof(itemId)" in region
assert "reclength < sizeof(ClipboardFormatItemRec)" in region
print("PASS clipboard format enumeration validates serialized bounds")
