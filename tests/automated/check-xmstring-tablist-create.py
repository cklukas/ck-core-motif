#!/usr/bin/env python3
"""Check XmString tab-list construction handles initial object failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
tab_create = source.index("tab = XmTabCreate(0.0, units, offset_model")
tab_guard = source.index("if (tab == NULL)", tab_create)
insert = source.index("tl = XmTabListInsertTabs(NULL, &tab, 1, 0);", tab_guard)
list_guard = source.index("if (tl == NULL)", insert)
assert tab_create < tab_guard < insert < list_guard
assert "XmTabFree(tab);" in source[list_guard:source.index("XmTabFree(tab);", list_guard) + 20]
print("PASS XmString tab-list construction handles initial failures")
