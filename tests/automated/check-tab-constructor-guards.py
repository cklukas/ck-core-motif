#!/usr/bin/env python3
"""Check tab-list widget constructors guard parent and list allocations."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()

list_start = text.index("Widget\n_XmCreateTabList")
list_end = text.index("/* \n * This copying routine", list_start)
list_block = text[list_start:list_end]
if "if (rend == NULL)" not in list_block:
    raise SystemExit("tab-list constructor does not guard its parent")
if "if (tl == NULL)" not in list_block:
    raise SystemExit("tab-list constructor does not guard allocation")

tab_start = text.index("Widget\n_XmCreateTab")
tab_end = text.index("void\nXmTabFree", tab_start)
tab_block = text[tab_start:tab_end]
if "if (tl == NULL)" not in tab_block:
    raise SystemExit("tab constructor does not guard its parent list")

print("tab-list widget constructors guard null state")
