#!/usr/bin/env python3
"""Check ScrolledWindow viewports remain inside tiny allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()
start = source.index("static void\nBoundViewport")
end = source.index("static void\nVariableLayout", start)
helper = source[start:end]
assert "MAX(0, MIN((int)*x" in helper
assert "MAX(1, MIN(*width" in helper
assert "MAX(1, MIN(*height" in helper
variable = source[source.index("static void\nVariableLayout"):source.index("static void\nConstantLayout")]
constant = source[source.index("static void\nConstantLayout"):]
assert "BoundViewport(sw, &newx, &newy, &childWidth, &childHeight);" in variable
assert "BoundViewport(sw, &clipX, &clipY, &clipWidth, &clipHeight);" in constant
print("ScrolledWindow viewports stay bounded inside tiny allocations")
