#!/usr/bin/env python3
"""Check DropDown internal child allocation failures are safe."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()
create_start = source.index("CreateChildren(Widget w")
create = source[create_start:source.index("SBBtnDownEH", create_start)]
assert "if (XmDropDown_label(cbw) != NULL)" in create
assert "if (XmDropDown_text(cbw) == NULL)" in create
assert "if (XmDropDown_arrow(cbw) == NULL)" in create
assert create.index("XmDropDown_text(cbw) == NULL") < create.index("XtAddCallback(XmDropDown_text(cbw)")
assert create.index("XmDropDown_arrow(cbw) == NULL") < create.index("XtSetValues(XmDropDown_arrow(cbw)")

query_start = source.index("QueryChildSize(Widget child")
query = source[query_start:source.index("FindDesiredSize", query_start)]
assert "if (child == NULL)" in query

place_start = source.index("static void\nPlaceChildren(Widget w, Boolean allow_resize, Widget child)")
place = source[place_start:source.index("static void\nGetTextAndLabelWidth", place_start)]
assert "if (text != NULL)" in place
assert "if (arrow != NULL)" in place
assert "label != NULL" in place

print("DropDown internal child allocation failures are null-safe")
