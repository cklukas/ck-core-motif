#!/usr/bin/env python3
"""Check ColorObj propagates pixel-property parse failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text()
start = source.index("FetchPixelData(Widget w")
end = source.index("static Boolean\nColorCachePropertyExists", start)
body = source[start:end]

assert "static Boolean" in source[source.index("static Boolean FetchPixelData") - 30:]
assert "return False;" in body
assert "return True;" in body
cache = source[end:source.index("/**********************************************************************/", end)]
assert "result = FetchPixelData" in cache
assert "return result;" in cache

print("ColorObj property parse status: ok")
