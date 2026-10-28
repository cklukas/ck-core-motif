#!/usr/bin/env python3
"""Check scaled XImage data is released with its allocating Xt API."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
helper_start = source.index("static void\nXmDestroyScaledImage")
function_start = source.index("void _XmPutScaledImage", helper_start)
helper = source[helper_start:function_start]
body = source[function_start : source.index("/*** COLOR CACHE NOW ***/", function_start)]
assert "XtFree(image->data);" in helper
assert "image->data = NULL;" in helper
assert "XDestroyImage(image);" in helper
assert "XDestroyImage(dest_image);" not in body
assert body.count("XmDestroyScaledImage(dest_image);") >= 4
print("PASS scaled XImage teardown matches Xt allocation")
