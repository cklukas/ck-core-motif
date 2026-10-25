#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/PanedW.c").read_text()
start = source.rindex("Initialize(\n")
end = source.index("/*************************************<->*************************************", start)
init = source[start:end]
assert "managed_children ? XmBLOCK : 0" in init
start = source.index(" *  ReManageChildren")
start = source.index("ReManageChildren(", start)
end = source.index("/*************************************<->*************************************", start)
region = source[start:end]
assert "SIZE_MAX / sizeof(Widget)" in region
assert "(Cardinal)~0 - XmBLOCK" in region
assert "UINT_MAX / sizeof(Widget)" in region
assert "(Cardinal)((size_t)new_slots * sizeof(Widget))" in region
assert "new_children = (WidgetList) XtRealloc" in region
assert "if (new_children == NULL)" in region
assert "return;" in region
print("PanedWindow managed-child allocation guards present")
