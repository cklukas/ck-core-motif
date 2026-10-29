#!/usr/bin/env python3
"""Check live ScrolledWindow scaling negotiates parent geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()
start = source.index("ScaleChanged(Widget widget")
end = source.index("/********************************", start)
function = source[start:end]
assert "XtMakeResizeRequest(widget, width, height" in function
assert "if (result == XtGeometryAlmost)" in function
assert "XtResizeWidget(widget, width, height" not in function
assert "sw->swindow.scale_last_width = sw->core.width" in function
assert "sw->swindow.scale_last_height = sw->core.height" in function
print("ScrolledWindow scale changes negotiate parent geometry")
