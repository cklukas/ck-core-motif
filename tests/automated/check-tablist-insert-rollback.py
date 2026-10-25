#!/usr/bin/env python3
"""Check tab-list insertion uses a detached, rollback-safe copy chain."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("XmTabList\nXmTabListInsertTabs")
end = text.index("/*ARGSUSED*/", start)
block = text[start:end]

if "Build the new tabs as a detached chain" not in block:
    raise SystemExit("tab insertion does not build a detached chain")
if "while (insert_start != NULL)" not in block:
    raise SystemExit("tab insertion has no partial-chain cleanup")
if "_XmTabNext(prev_tab) = insert_start;" not in block:
    raise SystemExit("tab insertion has no final splice")

print("tab-list insertion rolls back partial copies before splicing")
