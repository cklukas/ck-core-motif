#!/usr/bin/env python3
"""Check that converted and original image buffers have distinct cleanup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("/* Destroy non-cached XImage now that we've cached the pixmap. */")
end = source.index("    else if (image->format != old_image_format)", start)
region = source[start:end]
assert "XDestroyImage(image) ;" in region
assert "XtFree(old_image_data);" in region
assert "XtFree(image->data);" not in region
assert "size_t byte;" in source
assert "register int byte;" not in source
print("converted image cleanup frees the original buffer safely")
