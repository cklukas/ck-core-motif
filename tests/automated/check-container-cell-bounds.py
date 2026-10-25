#!/usr/bin/env python3
"""Check Container cell-count geometry uses signed available dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()

assert "((int)cw->core.width -" in source
assert "(2 * (int)cw->container.margin_w)" in source
assert "((int)cw->core.height -" in source
assert "(2 * (int)cw->container.margin_h)" in source
assert "if (width_in_cells <= 0 || height_in_cells <= 0)" in source

print("Container cell geometry is bounded")
