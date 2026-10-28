#!/usr/bin/env python3
"""Check accessibility text positions clamp both signed bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text()
start = source.index("static int bounded_text_position")
end = source.index("/* Live-object indexes", start)
region = source[start:end]

assert "value < (XmTextPosition)INT_MIN" in region
assert "return INT_MIN;" in region
assert "value > (XmTextPosition)INT_MAX" in region
print("accessibility text positions clamp both signed bounds")
