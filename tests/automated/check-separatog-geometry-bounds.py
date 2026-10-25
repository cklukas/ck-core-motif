#!/usr/bin/env python3
"""Check Separator gadget redisplay bounds collapsed inner geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SeparatoG.c").read_text()

assert "if (background_height < 0)" in source
assert "if (background_width < 0)" in source
assert "if (background_width > 0 && background_height > 0)" in source
assert "(Dimension) background_width" in source
assert "(Dimension) background_height" in source

print("Separator gadget geometry is bounded")
