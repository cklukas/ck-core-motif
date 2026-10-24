#!/usr/bin/env python3
"""Check that PNG decode storage is released on destination failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Png.c").read_text()
assert "if (!xdata) {\n        free(image_data);" in source
assert "if (!*ximage) {\n        free(xdata);\n        free(image_data);" in source
print("PNG destination allocation failures release decoded image data")
