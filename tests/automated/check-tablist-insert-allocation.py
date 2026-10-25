#!/usr/bin/env python3
"""Check tab-list insertion guards its container and first-tab allocations."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("XmTabListInsertTabs(XmTabList")
end = text.index("/* \n * This copying routine", start)
block = text[start:end]

if block.count("if (tl == NULL)") < 1:
    raise SystemExit("tab-list insertion does not guard new-list allocation")
if block.count("if (prev_tab == NULL)") < 1:
    raise SystemExit("tab-list insertion does not guard first-tab allocation")
if "if (tab == NULL)" not in block:
    raise SystemExit("tab-list insertion does not guard copied-list first tab")

print("tab-list insertion handles initial allocation failure")
