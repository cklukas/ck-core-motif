#!/usr/bin/env python3
"""Check TabList mutators reject masked NULL attribute arguments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
insert_start = source.index("XmTabbedStackListInsert(XmTabbedStackList tab_list")
insert_end = source.index("\n}\n", insert_start) + 2
insert = source[insert_start:insert_end]
assert "(mask != 0 && attributes == NULL)" in insert
assert insert.index("(mask != 0 && attributes == NULL)") < insert.index("attributes->")

modify_start = source.index("XmTabbedStackListModify(XmTabbedStackList tab_list")
modify_end = source.index("\n}\n", modify_start) + 2
modify = source[modify_start:modify_end]
assert "(mask != 0 && attributes == NULL)" in modify
assert modify.index("(mask != 0 && attributes == NULL)") < modify.index("attributes->")
print("PASS TabList argument guards")
