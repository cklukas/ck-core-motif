#!/usr/bin/env python3
"""Check XmScreen scratch-pixmap hash insertion failures clean up."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text()
get_start = source.index("_XmAllocScratchPixmap(")
get_start = source.rfind("\n", 0, get_start) + 1
get_end = source.index("\n}\n", get_start) + 2
get = source[get_start:get_end]
assert "if (!_XmAddHashEntry(inUseTable" in get
assert get.index("if (!_XmAddHashEntry(inUseTable") < get.index("XFreePixmap")
assert "XtFree((char *)returned_key);" in get

free_start = source.index("_XmFreeScratchPixmap(")
free_start = source.rfind("\n", 0, free_start) + 1
free_end = source.index("\n}\n", free_start) + 2
free = source[free_start:free_end]
assert "if (!_XmAddHashEntry(scratchTable" in free
assert free.index("if (!_XmAddHashEntry(scratchTable") < free.index("XFreePixmap")
assert "XtFree((char *)returned_key)" in free
print("PASS XmScreen scratch-cache failure cleanup")
