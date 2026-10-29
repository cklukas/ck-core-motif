#!/usr/bin/env python3
"""Check MultiList internal child allocation failures remain safe."""

from pathlib import Path


source = Path("lib/Xm/MultiList.c").read_text()

find_button_start = source.index("static Widget\nCreateFindButton")
find_button = source[find_button_start:source.index("static Widget\nCreateFindText", find_button_start)]
assert "if (w != NULL)" in find_button
assert find_button.index("if (w != NULL)") < find_button.index("XtAddCallback")

find_text_start = source.index("static Widget\nCreateFindText")
find_text = source[find_text_start:source.index("static Widget \nCreateFrame", find_text_start)]
assert "if (w != NULL)" in find_text
assert find_text.index("if (w != NULL)") < find_text.index("XtAddCallback")

frame_start = source.index("static Widget \nCreateFrame")
frame = source[frame_start:source.index("static Dimension\nmultilist_frame_dimension", frame_start)]
assert "if (w == NULL)" in frame
assert frame.index("XtCreateWidget") < frame.index("if (w == NULL)")
assert frame.index("if (w == NULL)") < frame.index("num_largs = 0;", frame.index("if (w == NULL)"))
assert "if (XmMultiList_ilist(elist) == NULL)" in frame
assert frame.index("if (XmMultiList_ilist(elist) == NULL)") < frame.index("XmMultiList_ilist(elist)->core.width")

initialize_start = source.index("static void Initialize(Widget req")
initialize = source[initialize_start:source.index("ClassPartInitialize", initialize_start)]
assert "XmMultiList_h_bar(elist) != NULL" in initialize
assert "XmMultiList_ilist(elist) != NULL && XmMultiList_v_bar(elist) != NULL" in initialize

layout_start = source.index("static void LayoutChildren(Widget w)")
layout = source[layout_start:source.index("static void PositionFindAndFindText", layout_start)]
assert "if (frame == NULL || XmMultiList_ilist(elist) == NULL" in layout

position_start = source.index("static void PositionFindAndFindText(Widget w)")
position = source[position_start:source.index("multilist_position", position_start)]
assert "if (find == NULL || find_text == NULL)" in position

print("MultiList internal child allocation failures are null-safe")
