#!/usr/bin/env python3
"""Check ColorSelector preview updates tolerate a missing preview widget."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text()

preferred_start = source.index("CalcPreferredSize(")
preferred = source[preferred_start:source.index("color_name_changed(XmColorSelectorWidget", preferred_start)]
assert "XmColorS_color_window(csw) != NULL" in preferred
assert "XtParent(XmColorS_color_window(csw))" in preferred

layout_start = source.index("compute_size(XmColorSelectorWidget csw)")
layout = source[layout_start:source.index("read_rgb_file(XmColorSelectorWidget csw", layout_start)]
assert "if (XmColorS_color_window(csw) != NULL)" in layout
assert "_XmConfigureWidget(XtParent(XmColorS_color_window(csw))" in layout

for name in ("NoPrivateColormaps", "PrivateColormaps"):
    marker = "static void\n" + name
    start = source.index(marker)
    end = source.index("/*", start + len(marker))
    body = source[start:end]
    assert "if (XmColorS_color_window(csw) != NULL)" in body
    assert "XtSetValues(XmColorS_color_window(csw)" in body

print("ColorSelector preview updates are null-safe")
