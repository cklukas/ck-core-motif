#!/usr/bin/env python3
"""Check ScrolledWindow handles a missing internal clip child safely."""

from pathlib import Path


source = Path("lib/Xm/ScrolledW.c").read_text()

initialize_start = source.index("new_w->swindow.ClipWindow =")
initialize = source[initialize_start:source.index("/* turn off the InsertChild flag */", initialize_start)]
assert "if (new_w->swindow.ClipWindow != NULL)" in initialize
assert "scroll_frame != NULL && scroll_frame->init != NULL" in initialize

resize_start = source.index("Resize(\nWidget wid")
resize = source[resize_start:source.index("GetVariableSize", resize_start)]
assert "if (sw->swindow.ClipWindow == NULL)" in resize
assert resize.index("ClipWindow == NULL") < resize.index("VariableLayout(sw)")

constant_start = source.index("ConstantLayout(\nXmScrolledWindowWidget sw")
constant = source[constant_start:source.index("VariableLayout(", constant_start + 1)]
assert "if (clip == NULL)" in constant

slider_start = source.index("SliderMove(\nWidget w")
slider = source[slider_start:]
assert "if (clip == NULL)" in slider

print("ScrolledWindow clip allocation failures are null-safe")
