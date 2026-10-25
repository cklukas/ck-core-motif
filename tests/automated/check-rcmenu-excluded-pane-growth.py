#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/RCMenu.c").read_text()
start = source.index("ButtonMenuPopDown(")
end = source.index("excPP->num_panes", start)
region = source[start:end]
assert "int depth;" in region
assert "depth >= SHRT_MAX" in region
assert "SIZE_MAX / sizeof(Widget)" in region
assert "UINT_MAX / sizeof(Widget)" in region
assert "Widget *new_pane" in region
assert "if (new_pane == NULL)" in region
assert region.index("if (new_pane == NULL)") < region.index("excPP->pane = new_pane")
assert "pane_list_size = (short)new_size" in region
print("RCMenu excluded-pane growth guards present")
