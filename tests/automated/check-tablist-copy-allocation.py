#!/usr/bin/env python3
"""Check tab-list copying guards list and first-tab allocation."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("TabListCopyUnlocked(XmTabList")
end = text.index("/* \n * Marked tabs", start)
block = text[start:end]

for needle in (
    "if (tl == NULL)",
    "if (count == 0)",
    "if (tab == NULL)",
    "XtFree((char *)tl);",
):
    if needle not in block:
        raise SystemExit(f"tab-list copy is missing {needle}")

print("tab-list copying handles initial allocation failure")
