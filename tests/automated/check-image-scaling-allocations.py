#!/usr/bin/env python3
"""Check that image scaling handles destination allocation failure safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("\n\n\n/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "Table table = { NULL, NULL, NULL, NULL };" in region
assert "if (dest_image->data == NULL)" in region
assert "if (table.x == NULL || table.y == NULL" in region
assert "goto cleanup;" in region
print("image scaling checks destination and lookup-table allocations")
