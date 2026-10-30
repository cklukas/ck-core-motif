#!/usr/bin/env python3
"""Check ColorSelector internal child allocation failures are safe."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text()

sliders_start = source.index("static void\nCreateColorSliders")
sliders = source[sliders_start:source.index("static void\nCreateSelectorRadio", sliders_start)]
assert "if (XmColorS_bb(csw) == NULL)" in sliders
assert "if (XmColorS_sliders(csw)[i] == NULL)" in sliders
assert sliders.index("XmColorS_sliders(csw)[i] == NULL") < sliders.index("XtAddCallback")

radio_start = source.index("static void\nCreateSelectorRadio")
radio = source[radio_start:source.index("static void\nCreateColorWindow", radio_start)]
assert "if (w == NULL)" in radio
assert "if (w != NULL)" in radio

window_start = source.index("static void\nCreateColorWindow")
window = source[window_start:source.index("GetValues_XmNredSliderLabel", window_start)]
assert "if (fr == NULL)" in window

sliders_set_start = source.index("static void\nSetSliders(XmColorSelectorWidget")
sliders_set = source[sliders_set_start:source.index("static void", sliders_set_start + 1)]
assert sliders_set.count("!= NULL") >= 3

layout_start = source.index("compute_size(XmColorSelectorWidget csw)")
layout = source[layout_start:layout_start + 700]
assert "XmColorS_bb(csw) == NULL" in layout

print("ColorSelector internal child allocation failures are null-safe")
