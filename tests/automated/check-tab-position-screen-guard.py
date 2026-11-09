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
for needle in (
    "float tab_value;",
    "tab_value >= (float)INT_MAX",
    "tab_value <= (float)INT_MIN",
    "previous > SHRT_MAX - converted",
    "previous < SHRT_MIN - converted",
    "if (converted > SHRT_MAX)",
    "if (converted < SHRT_MIN)",
    "return (Position)converted;",
):
    if needle not in window:
        raise SystemExit(f"tab position conversion is missing {needle}")

print("tab-list position conversion guards screen and legacy coordinate bounds")
