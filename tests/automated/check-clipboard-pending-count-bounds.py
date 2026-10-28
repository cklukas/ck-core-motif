#!/usr/bin/env python3
"""Check clipboard item counts fit iteration and Xt allocation types."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
validator_start = source.index("ClipboardHeaderRecordValid(", source.index("ClipboardHeaderRecordValid(") + 1)
validator = source[validator_start:source.index("ClipboardOpen(", validator_start)]
pending_start = source.index("XmClipboardInquirePendingItems(")
pending = source[pending_start:source.index("XmClipboardRegisterFormat(", pending_start)]

assert "header->currItems > (unsigned long)INT_MAX" in validator
assert "header->currItems > (unsigned long)UINT_MAX /" in pending
assert "for ( i = 0; i < header->currItems; i++ )" in pending

print("Clipboard pending-item counts fit iteration and allocation bounds")
