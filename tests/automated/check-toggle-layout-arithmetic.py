#!/usr/bin/env python3
"""Check ToggleButton layout arithmetic uses signed dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text()

assert "(short)((int)new_w->core.width" in source
assert "(int)new_w->core.height - (int)new_w->label.margin_top" in source
assert "(int)w->core.width - (int)w->primitive.highlight_thickness" in source
assert "((int)w->core.height - (int)w->toggle.indicator_dim) / 2" in source

print("ToggleButton layout geometry uses signed arithmetic")
