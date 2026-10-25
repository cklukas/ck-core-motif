#!/usr/bin/env python3
"""Check proposed XmString tab-list setup handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableProposeTablist(")
tab = source.index("tab = XmTabCreate", start)
guard = source.index("if (tab == NULL)", tab)
insert = source.index("tl = XmTabListInsertTabs", guard)
list_guard = source.index("if (tl == NULL)", insert)
assert tab < guard < insert < list_guard
assert "XmTabFree(tab);" in source[list_guard:source.index("XmTabFree(tab);", list_guard) + 20]
print("PASS proposed XmString tab-list setup is guarded")
