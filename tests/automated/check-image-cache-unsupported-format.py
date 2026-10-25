#!/usr/bin/env python3
"""Check image-cache unsupported formats use cleanup instead of asserting."""

from pathlib import Path


source = Path("lib/Xm/ImageCache.c").read_text()
start = source.index("if (image->depth == 1)")
end = source.index("\n    /*\n     * Create a pixmap", start)
region = source[start:end]

assert "default:" in region
default = region.index("default:")
tail = region[default:]
assert "DiscardScaledPixmapBuild(ret, image, old_image_data," in tail
assert "return (XmUNSPECIFIED_PIXMAP);" in tail
assert "assert(FALSE);" not in tail

print("image-cache unsupported formats clean up safely")
