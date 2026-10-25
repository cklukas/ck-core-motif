#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/CascadeB.c").read_text()
start = source.index("_XmCascadingPopup(")
end = source.index("if (XmIsCascadeButtonGadget(cb))", start)
region = source[start:end]
assert "excPP->pane == NULL || excPP->pane_list_size < 1" in region
assert "Widget *new_pane" in region
assert "XtRealloc" in region
assert "if (new_pane == NULL)" in region
assert region.index("if (new_pane == NULL)") < region.index("excPP->pane = new_pane")
assert "excPP->pane_list_size = 4" in region
print("CascadeButton excluded-pane initialization guard present")
