#!/usr/bin/env python3
"""Check that direct clipboard header consumers use shared validation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
send_start = source.index("static int \nClipboardSendMessage(")
send_end = source.index("static int \nClipboardDataIsReady(", send_start)
copy_start = source.index("int \nXmClipboardCopyByName(")
copy_end = source.index("int \nXmClipboardUndoCopy(", copy_start)

assert "ClipboardHeaderRecordValid(root_clipboard_header, headerlength)" in source[send_start:send_end]
assert "ClipboardHeaderRecordValid(root_clipboard_header, headerlength)" in source[copy_start:copy_end]
print("PASS direct clipboard header consumers validate list bounds")
