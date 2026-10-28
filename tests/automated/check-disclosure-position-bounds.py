#!/usr/bin/env python3
"""Check Disclosure layout origins stay within Position."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Disclosure.c").read_text()

assert "static Position position(long long value)" in source
assert "if (value < SHRT_MIN)" in source
assert "if (value > SHRT_MAX)" in source
layout = source[source.index("static void layout") : source.index("static void negotiate")]
assert "x = position(w->disclosure.margin_width);" in layout
assert "y = position(w->disclosure.margin_height);" in layout
assert "position((long long)x + width - aw)" in layout
assert "position((long long)x + aw)" in layout
assert "y = position((long long)y + head" in layout
assert "(Position)(x + width - aw)" not in layout
assert "(Position)(x + aw)" not in layout
print("PASS Disclosure layout origins are bounded")
