#!/usr/bin/env python3
"""Check ArrowButton default geometry cannot wrap Dimension values."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ArrowB.c").read_text()

assert "static Dimension AddDefaultDimension(Dimension value);" in source
assert "new_w->core.width = AddDefaultDimension(new_w->core.width);" in source
assert "new_w->core.height = AddDefaultDimension(new_w->core.height);" in source
helper = source[source.index("static Dimension\nAddDefaultDimension") :]
assert "USHRT_MAX - 15" in helper
assert "(Dimension)USHRT_MAX" in helper
print("PASS ArrowButton default dimensions saturate")
