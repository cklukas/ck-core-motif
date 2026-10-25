#!/usr/bin/env python3
"""Check that XmeGetMask allocates names for arbitrary image paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("XmeGetMask(")
end = source.index("\n\n\n/************************************************************************", start)
region = source[start:end]
assert "String \t\tmask_name;" in region
assert "if (screen == NULL || image_name == NULL)" in region
assert "XmMaskNameSize(image_name, &mask_name_size)" in region
assert "mask_name = XtMalloc((Cardinal)mask_name_size);" in region
assert "XtFree(mask_name);" in region
assert "char \t\tmask_name[256];" not in region
print("XmeGetMask handles long names and null inputs safely")
