#!/usr/bin/env python3
"""Check Container dynamic detail-tab width uses signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()

assert "tab_size = (int)cw->core.width" in source
assert "(2 * (int)cw->container.margin_w)" in source
assert "(int)cw->container.real_first_col_width" in source

print("Container dynamic tab width arithmetic is bounded")
