#!/usr/bin/env python3
"""Check that the final scaled-image strip is clamped before addition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
region = source[start:end]

guard = "if (strip_height > source_end_y - min_y)"
assert guard in region
assert "max_y = min_y + strip_height;" in region
assert region.index(guard) < region.index("max_y = min_y + strip_height;")
print("scaled image strip endpoint is clamped before addition")
