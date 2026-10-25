#!/usr/bin/env python3
"""Check ScrollBar redisplay shadow spans remain bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollBar.c").read_text()

assert "Dimension shadow_width, shadow_height;" in source
assert "shadow_width = (sbw->core.width >" in source
assert "shadow_height = (sbw->core.height >" in source
assert "sbw->core.width-2 *" not in source
assert "sbw->core.height-2 *" not in source

print("ScrollBar redisplay shadow spans are bounded")
