#!/usr/bin/env python3
"""Check BreadcrumbBar rejects counts beyond its signed current-index range."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text()
start = source.index("void XmBreadcrumbBarSetSegments")
end = source.index("\n}\n", start)
body = source[start:end]

assert "count > (Cardinal)INT_MAX" in body
assert body.index("count > (Cardinal)INT_MAX") < body.index("bar =")

print("BreadcrumbBar rejects unrepresentable segment counts")
