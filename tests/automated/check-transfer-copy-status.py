#!/usr/bin/env python3
"""Check that clipboard transfer bookkeeping follows copy status."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
assert source.count("clipboard_status = XmClipboardCopy") == 2
assert source.count("if (clipboard_status == ClipboardSuccess)") == 2
assert "format2 == 32" in source
assert "if (req != NULL && registered == 0)" in source
print("clipboard bookkeeping requires successful copy operations")
