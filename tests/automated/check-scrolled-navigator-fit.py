#!/usr/bin/env python3
"""Check automatic ScrolledWindow navigators yield to tiny allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()
start = source.rindex("static void\nConstantLayout")
end = source.index("static void\nResize", start)
function = source[start:end]
assert "MyHeight <= (int)sw->swindow.hScrollBar->core.height" in function
assert "MyWidth <= (int)sw->swindow.vScrollBar->core.width" in function
assert "HSBExists = False;" in function
assert "VSBExists = False;" in function
print("ScrolledWindow automatic navigators yield to tiny allocations")
