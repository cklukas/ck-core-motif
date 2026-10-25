#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/RCMenu.c").read_text()
start = source.rindex("AddKeycodeToKeyboardList(")
end = source.index("RemoveFromKeyboardList(", start)
region = source[start:end]
assert "old_size = MGR_SizeKeyboardList(rowcol)" in region
assert "old_size > SHRT_MAX - 10" in region
assert "SIZE_MAX / sizeof(XmKeyboardData)" in region
assert "UINT_MAX / sizeof(XmKeyboardData)" in region
assert "XmKeyboardData *new_list" in region
assert "if (new_list == NULL)" in region
assert "MGR_KeyboardList(rowcol) = new_list;" in region
assert "MGR_SizeKeyboardList(rowcol) = new_size;" in region
assert region.index("if (new_list == NULL)") < region.index("MGR_KeyboardList(rowcol) = new_list;")
print("RCMenu keyboard-list growth guards present")
