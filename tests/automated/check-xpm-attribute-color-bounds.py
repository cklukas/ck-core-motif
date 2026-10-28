#!/usr/bin/env python3
"""Check XPM image scanning validates caller-supplied color metadata."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text(
    encoding="utf-8"
)
start = source.index("if (display == NULL || xpmimage == NULL")
region = source[start:source.index("/* initialize pmap */", start)]
assert "XpmColorTable | XpmInfos" in region
assert "attributes->colorTable == NULL" in region
assert "attributes->pixels == NULL" in region
assert "attributes->mask_pixel >= attributes->ncolors" in region
assert "return XpmFileInvalid;" in region
print("PASS XPM color metadata bounds are validated")
