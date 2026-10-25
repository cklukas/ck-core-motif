#!/usr/bin/env python3
"""Check shared separator drawing rejects margins larger than the span."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()

assert "int i, ndash, shadow_dash_size, separator_span;" in source
assert "separator_span = (orientation == XmHORIZONTAL" in source
assert "if (separator_span <= 0)" in source
assert "shadow_dash_size = separator_span;" in source
assert "< (width - 2*margin)" not in source
assert "< (height - 2*margin)" not in source

print("shared separator shadow spans are bounded")
