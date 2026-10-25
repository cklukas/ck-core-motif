#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Container.c").read_text()
start = source.index("XmContainerGetItemChildren(")
end = source.index("XmContainerRelayout(", start)
region = source[start:end]
assert "clist = (WidgetList)XtMalloc((Cardinal)(sizeof(Widget) *" in region
assert "UINT_MAX / sizeof(Widget)" in region
assert "if (clist == NULL)" in region
assert region.index("if (clist == NULL)") < region.index("clist[i] = node->widget_ptr")
assert "_XmAppUnlock(app);" in region
print("Container child-list allocation guard present")
