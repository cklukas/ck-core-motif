#!/usr/bin/env python3
"""Check List and ComboBox callback lengths preserve their int ABI safely."""
from pathlib import Path

root = Path(__file__).parents[2]
list_source = (root / "lib" / "Xm" / "List.c").read_text()
combo_source = (root / "lib" / "Xm" / "ComboBox.c").read_text()

list_start = list_source.index("static void\nDefaultAction")
list_end = list_source.index("static void\nClickElement", list_start)
default_region = list_source[list_start:list_end]
click_start = list_end
click_end = list_source.index("static void\n", click_start + 20)
click_region = list_source[click_start:click_end]

for region in (default_region, click_region):
    assert "unsigned int item_length = XmStringLength" in region
    assert "item_length > (unsigned int)INT_MAX ? INT_MAX : (int)item_length" in region

combo_start = combo_source.index("XmString item = GetEditBoxValue")
combo_end = combo_source.index("XtCallCallbacks", combo_start)
combo_region = combo_source[combo_start:combo_end]
assert "item_length = XmStringLength(item)" in combo_region
assert "item_length > (unsigned int)INT_MAX ? INT_MAX" in combo_region
print("List and ComboBox callback string lengths are bounded at the int ABI")
