#!/usr/bin/env python3
"""Check Container and Notebook control-flow scopes stay explicit."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]
container = (root / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
notebook = (root / "lib" / "Xm" / "Notebook.c").read_text(encoding="utf-8")

place_start = container.rindex("PlaceCwid(\n")
place = container[place_start : container.index("SnapCwid(\n", place_start)]
assert "    else {\n\tx = MAX(x,AdjustPositionSaturated" in place
assert "\n\t}\n\ty = MAX(y,AdjustPositionSaturated" in place

for signature in (
    "GeometryManager(\n",
    "SetVisualEmphasis(\n",
    "SetViewType(\n",
    "GetDefaultDetailCount(\n",
    "SetDynamicTabList(\n",
    "GetDynFirstColWidth(\n",
    "ResizeIconWidths(\n",
    "RequestOutlineDetail(\n",
    "LayoutOutlineDetail(\n",
    "LayoutSpatial(\n",
    "SetSmallCellSizes(\n",
    "SetLargeCellSizes(\n",
    "SizeOutlineButton(\n",
    "CreateIconHeader(\n",
    "GetRealIconHeader(\n",
    "UpdateIconHeader(\n",
):
    start = container.rindex(signature)
    region = container[start : start + 900]
    assert "\n    cw = " in region, signature

geometry_start = container.rindex("GeometryManager(\n")
geometry = container[geometry_start : geometry_start + 700]
assert "\n    wid = XtParent(cwid);" in geometry
assert "\n    if (wid == NULL)" in geometry

for signature in ("GetNextTraversalWidget(\n", "GetPrevTraversalWidget(\n"):
    start = container.rindex(signature)
    region = container[start : start + 650]
    assert "\n    c = GetContainerConstraint(child);" in region, signature
    assert "\n    child_node = c->node_ptr;" in region, signature

next_tab_start = notebook.rindex("GetNextTab (\n")
next_tab = notebook[next_tab_start : notebook.index("MaxIsRightUp (\n", next_tab_start)]
major_condition = "if ((nc->active) && (NB_IS_CHILD_MAJOR(nc->child_type))) {"
minor_condition = "else if ((nc->active)"
assert major_condition in next_tab
assert minor_condition in next_tab
assert next_tab.index("} /* switch */\n\t  }\n\t    i++;") > next_tab.index(major_condition)
assert next_tab.index("} /* switch */\n\t    }\n\t    i++;") > next_tab.index(minor_condition)

print("PASS Container and Notebook control-flow scopes are explicit")
