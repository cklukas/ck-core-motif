#!/usr/bin/env python3
"""Check XmTabList insertion and cleanup use full-width count arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c").read_text(
    encoding="utf-8"
)
insert_start = source.index("XmTabList\nXmTabListInsertTabs")
insert_end = source.index("\n/*", insert_start)
insert = source[insert_start:insert_end]
assert "tab_count > (Cardinal)~0 - _XmTabLCount(oldlist)" in insert
assert "return(NULL);" in insert
free_start = source.index("void\nXmTabListFree")
free_end = source.index("\nCardinal\nXmTabListTabCount", free_start)
free = source[free_start:free_end]
assert "Cardinal i;" in free
assert "int\ti;" not in free
print("XmTabList count growth and cleanup are full-width")
