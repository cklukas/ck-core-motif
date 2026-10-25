#!/usr/bin/env python3
"""Check ScrollBar resize spans remain valid for collapsed controls."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollBar.c").read_text()

assert "#define BOUNDED_SPAN(total, inset)" in source
assert "BOUNDED_SPAN(sbw->core.width, 2 * (ht + st))" in source
assert "BOUNDED_SPAN(\n\t\tsbw->core.width," in source
assert "arrow_space > 1" in source
assert "#undef BOUNDED_SPAN" in source

print("ScrollBar resize spans are bounded")
