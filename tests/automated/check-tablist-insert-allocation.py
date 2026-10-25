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
if "if (tl == NULL)" not in block:
    raise SystemExit("tab-list insertion does not guard copied-list allocation")
if "tab = _XmTabCopy(tabs[i]);" not in block or "if (tab == NULL)" not in block:
    raise SystemExit("tab-list insertion does not guard copied tab allocation")

print("tab-list insertion handles initial allocation failure")
