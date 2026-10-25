#!/usr/bin/env python3
"""Check that MenuState allocation failure is handled by menu helpers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MenuState.c").read_text(encoding="utf-8")
start = source.index("_XmSetInDragMode(")
end = source.index("/************************************************************************\n *\n * _XmGetMenuState", start)
region = source[start:end]
assert "XmMenuState menu_state = _XmGetMenuState(widget);" in region
assert "if (menu_state == NULL)" in region
get_start = source.index("XmMenuState\n_XmGetMenuState(\n")
get_end = source.index("/*ARGSUSED*/", get_start)
get_region = source[get_start:get_end]
assert "menu_state = (XmMenuState)XtMalloc(sizeof(XmMenuStateRec));" in get_region
assert "if (menu_state == NULL)" in get_region
assert "return (XmMenuState)NULL;" in get_region
print("PASS MenuState allocation failure is guarded")
