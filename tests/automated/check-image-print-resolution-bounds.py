#!/usr/bin/env python3
"""Check that print-resolution narrowing is range checked."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("#ifdef PRINTING_SUPPORTED", source.index("void _XmPutScaledImage"))
end = source.index("#endif /* PRINTING_SUPPORTED */", start)
region = source[start:end]

assert "double image_res_value" in region
assert "isfinite(image_res_value)" in region
assert "image_res_value <= (double)INT_MAX" in region
assert "int image_res = (int)image_res_value" in region
assert region.index("image_res_value <= (double)INT_MAX") < region.index("int image_res = (int)image_res_value")
print("scaled-image print resolution is range checked before int conversion")
