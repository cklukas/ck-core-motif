#!/usr/bin/env python3
"""Check tab-list position conversion validates its Screen input."""

from pathlib import Path


source = Path("lib/Xm/XmTabList.c").read_text()
start = source.index("_XmTabListGetPosition(")
end = source.index("#ifdef _XmDEBUG_XMTABLIST", start)
window = source[start:end]

assert "if (screen == NULL)" in window
assert "return 0;" in window
assert "_XmConvertUnits(screen" in window

print("tab-list position conversion guards null screens")
