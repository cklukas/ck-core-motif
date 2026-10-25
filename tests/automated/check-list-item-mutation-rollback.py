#!/usr/bin/env python3
"""Check XmList insertion and replacement preserve ownership on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

add_start = source.index("AddItems(XmListWidget lw,", source.index("AddItems - add items"))
add_end = source.index("\n}\n", add_start) + 2
add = source[add_start:add_end]
assert "UINT_MAX / sizeof(*new_items)" in add
assert "if (lw->list.items[pos + i] == NULL)" in add
assert "while (i != 0)" in add
assert "XmStringFree(lw->list.items[pos + --i])" in add
assert "memmove((char *) (lw->list.items + pos)" in add
assert "return False;" in add

replace_start = source.index("ReplaceItem(XmListWidget lw,", source.index("ReplaceItem - Replace"))
replace_end = source.index("\n}\n", replace_start) + 2
replace = source[replace_start:replace_end]
assert "replacement = XmStringCopy(item);" in replace
assert "if (replacement == NULL)" in replace
assert "XmStringFree(lw->list.items[pos]);" in replace
assert "selected_replacement = XmStringCopy(item);" in replace
assert "if (selected_replacement != NULL)" in replace
print("XmList item mutation paths preserve ownership on copy failure")
