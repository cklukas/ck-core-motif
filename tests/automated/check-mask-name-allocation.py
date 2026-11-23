#!/usr/bin/env python3
"""Check that the shared mask lookup allocates names for arbitrary image paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("\nXmeGetScaledMask(")
region = source[start:source.index("\n}", start)]
assert "if (screen == NULL || image_name == NULL)" in region
assert "XmMaskNameSize(image_name, &mask_name_size)" in region
assert "mask_name = XtMalloc((Cardinal)mask_name_size);" in region
assert "if (mask_name == NULL)" in region
assert region.index("if (mask_name == NULL)") < region.index("_XmOSGenerateMaskName(")
assert region.index("_XmGetScaledPixmap(") < region.index("XtFree(mask_name);")
assert "XtFree(mask_name);" in region
wrapper = source.index("\nXmeGetMask(")
assert "return XmeGetScaledMask(screen, NULL, image_name, 1.0);" in source[wrapper:source.index("\n}", wrapper)]
print("Mask lookup handles long names and null inputs safely")
