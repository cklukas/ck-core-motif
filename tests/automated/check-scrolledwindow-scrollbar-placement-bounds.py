#!/usr/bin/env python3
"""Check ScrolledWindow scrollbar placement handles collapsed geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()

assert "int vsb_pos, hsb_pos;" in source
assert "(int) sw->core.width - (int) sw->swindow.WidthPad" in source
assert "(int) sw->core.height - (int) sw->swindow.HeightPad" in source
assert "(vsb_pos > 0) ? vsb_pos : 0" in source
assert "(hsb_pos > 0) ? hsb_pos : 0" in source

print("ScrolledWindow scrollbar placement is bounded")
