#!/usr/bin/env python3
"""Check XmTabbedStackList insertion growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
start = source.index("XmTabbedStackListInsert(XmTabbedStackList tab_list")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "newTabs = (XmTabAttributes)XtRealloc" in region
assert "if (newTabs == NULL)" in region
assert region.index("if (newTabs == NULL)") < region.index("tab_list->tabs = newTabs")
assert region.index("tab_list->tabs = newTabs") < region.index("tab_list->allocated = newAllocated")
assert "SIZE_MAX / sizeof(XmTabAttributeRec)" in region
assert "tab_list->allocated += XmLIST_GROWTH_FACTOR" not in region
assert "tab_list->tabs = (XmTabAttributes)\n\t    XtRealloc" not in region
print("PASS TabList growth transaction")
