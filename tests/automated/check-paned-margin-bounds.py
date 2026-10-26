#!/usr/bin/env python3
"""Check Paned margin changes use bounded signed Dimension arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PanedW.c").read_text()

assert "static Dimension AdjustDimensionDelta(Dimension value, int64_t delta);" in source
assert "int64_t margin_delta =" in source
assert "2 * (int64_t)newpw->paned_window.margin_width" in source
assert "newpw->core.width = AdjustDimensionDelta" in source
helper = source[source.index("static Dimension\nAdjustDimensionDelta") :]
assert "adjusted <= 0" in helper
assert "adjusted >= USHRT_MAX" in helper
print("PASS Paned margin geometry is bounded")
