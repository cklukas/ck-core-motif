#!/usr/bin/env python3
"""Check XmeClearBorder clamps oversized border thickness."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()

assert "int clear_width, clear_height, border;" in source
assert "if (border > clear_width)" in source
assert "if (border > clear_height)" in source
assert "clear_height - border" in source
assert "clear_width - border" in source
assert "height - shadow_thick" not in source
assert "width - shadow_thick" not in source

print("XmeClearBorder spans are bounded")
