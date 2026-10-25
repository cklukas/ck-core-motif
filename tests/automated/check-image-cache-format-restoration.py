#!/usr/bin/env python3
"""Check image-cache format restoration does not assert on metadata."""

from pathlib import Path


source = Path("lib/Xm/ImageCache.c").read_text()
start = source.index("else if (image->format != old_image_format)")
end = source.index("\n    return (pixmap);", start)
region = source[start:end]

assert "image->format = old_image_format;" in region
assert "assert(old_image_format == XYPixmap);" not in region
assert "_XmProcessUnlock();" in region

print("image-cache format restoration is assertion-free")
