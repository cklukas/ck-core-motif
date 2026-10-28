#!/usr/bin/env python3
"""Check converted XImage buffers use allocator-matched teardown."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
helper_start = source.index("static void\nXmDestroyConvertedImage")
helper = source[helper_start : source.index("void _XmPutScaledImage", helper_start)]
function_start = source.index("DiscardScaledPixmapBuild(")
region = source[function_start : source.index("/*** COLOR CACHE NOW ***/", function_start)]
assert "if (xt_data_owned && image->data != NULL)" in helper
assert "XtFree(image->data);" in helper
assert "image->data = NULL;" in helper
assert "XDestroyImage(image);" in helper
assert "XmDestroyConvertedImage(image, old_image_data != NULL);" in region
assert "image->data == old_image_data" in region
assert "image->data = NULL;" in region
assert "XDestroyImage(image) ;" not in region
print("PASS converted XImage teardown preserves allocator ownership")
