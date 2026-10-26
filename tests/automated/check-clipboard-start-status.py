#!/usr/bin/env python3
"""Check that clipboard-source setup requires a successful start."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(")
end = source.index("/*************************************************************/\n/* Transfer routine section", start)
region = source[start:end]
assert "status = XmClipboardStartCopy" in region
assert "if (status != XmClipboardSuccess)" in region
assert "cc -> itemid = itemid;" in region
print("clipboard-source setup requires successful start before using item id")
