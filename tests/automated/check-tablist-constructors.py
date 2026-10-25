#!/usr/bin/env python3
"""Check TabList constructors handle allocation failure safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
create_start = source.index("XmTabbedStackListCreate(void)")
create_end = source.index("\n}\n", create_start) + 2
create = source[create_start:create_end]
assert "if (tabList == NULL)" in create
assert create.index("if (tabList == NULL)") < create.index("tabList->allocated")

copy_start = source.index("XmTabbedStackListCopy(XmTabbedStackList tab_list)")
copy_end = source.index("\n}\n", copy_start) + 2
copy = source[copy_start:copy_end]
assert "if (newList == NULL)" in copy
assert "if (newList->tabs == NULL)" in copy
assert "SIZE_MAX / sizeof(XmTabAttributeRec)" in copy
assert copy.index("if (newList->tabs == NULL)") < copy.index("newList->tabs[i]")
assert "XtFree((XtPointer)newList);" in copy
print("PASS TabList constructor allocation guards")
