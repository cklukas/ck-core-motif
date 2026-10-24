#!/usr/bin/env python3
"""Check that IconG mask names are dynamically sized."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()
start = source.index("/* fetch only if a mask has not been specified")
end = source.index("\n   }\n}\n", start)
region = source[start:end]
assert "String mask_name;" in region
assert "mask_name = XtMalloc(strlen(image_name) + 3);" in region
assert "if (mask_name == NULL)" in region
assert "XtFree(mask_name);" in region
assert "char mask_name[255]" not in region
print("IconG mask names are dynamically bounded")
