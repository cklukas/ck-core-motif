#!/usr/bin/env python3
"""Check TabStack default geometry cannot wrap Dimension values."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text()

assert "static Dimension TabStackDefaultDimension(Dimension shadow, Dimension margin);" in source
assert "ts->core.width = TabStackDefaultDimension(" in source
assert "ts->core.height = TabStackDefaultDimension(" in source
helper = source[source.index("static Dimension\nTabStackDefaultDimension") :]
assert "uint64_t total =" in helper
assert "total > USHRT_MAX" in helper
print("PASS TabStack default dimensions saturate")
