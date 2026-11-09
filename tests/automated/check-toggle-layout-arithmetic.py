#!/usr/bin/env python3
"""Check ToggleButton layout arithmetic uses signed dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text()

assert "new_w->label.acc_TextRect.x = toggle_bounded_position(" in source
assert "(int64_t)new_w->core.height - new_w->label.margin_top" in source
assert "(int)w->core.width - (int)w->primitive.highlight_thickness" in source
assert "((int)w->core.height - (int)w->toggle.indicator_dim) / 2" in source

print("ToggleButton layout geometry uses signed arithmetic")
