#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/RCMenu.c").read_text()
start = source.index("_XmRC_AddToPostFromList(")
end = source.index("_XmRC_RemoveFromPostFromListOnDestroyCB", start)
region = source[start:end]
assert "postFromCount >= m->row_column.postFromListSize" in region
assert "postFromCount < 0" in region
assert "postFromListSize < 0" in region
assert "SIZE_MAX / sizeof(Widget)" in region
assert "UINT_MAX / sizeof(Widget)" in region
assert "Widget *new_list" in region
assert "if (new_list == NULL)" in region
assert region.index("if (new_list == NULL)") < region.index("postFromList = new_list")
assert "postFromListSize = new_size" in region
print("RCMenu post-from-list growth guards present")
