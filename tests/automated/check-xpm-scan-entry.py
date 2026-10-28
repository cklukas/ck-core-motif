#!/usr/bin/env python3
"""Check XPM image scanning validates public entry arguments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text()
start = source.index("XpmCreateXpmImageFromImage(\n")
start = source.index("{", start)
end = source.index("\n}", start)
function = source[start:end]
guard = "display == NULL || xpmimage == NULL ||\n\t(image == NULL && shapeimage == NULL)"
assert guard in function
assert function.index(guard) < function.index("/* initialize pmap */")
assert "if (width == 0 || height == 0)" in function
assert function.index("if (width == 0 || height == 0)") < function.rindex("pmap.pixelindex =")
assert "image->data == NULL" in function
assert "shapeimage->data == NULL" in function
assert "image->bytes_per_line <= 0" in function
assert "shapeimage->bytes_per_line <= 0" in function
assert "image->bitmap_unit <= 0" in function
assert "image->bits_per_pixel <= 0" in function
assert "shapeimage->bitmap_unit <= 0" in function
assert "shapeimage->bits_per_pixel <= 0" in function
assert "image->depth <= 0" in function
assert "shapeimage->depth <= 0" in function
assert "image->xoffset < 0" in function
assert "shapeimage->xoffset < 0" in function
print("PASS XPM image-scan entry guard")
