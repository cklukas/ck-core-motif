#!/usr/bin/env python3
"""Check that clipboard header list offsets and counts are bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static Boolean\nClipboardHeaderRecordValid(")
end = source.index("static void \nClipboardClose(", start)
region = source[start:end]

assert "header->dataItemList > ULONG_MAX / CONVERT_32_FACTOR" in region
assert "list_offset > length" in region
assert "header->currItems > (length - list_offset) / sizeof(itemId)" in region
assert "ClipboardHeaderRecordValid(root_clipboard_header, headerlength)" in region
print("PASS clipboard header list bounds are validated")
