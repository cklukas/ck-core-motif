#!/usr/bin/env python3
"""Check Container cell preferred-size division guard."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("GetSpatialSize (", source.index("* GetSpatialSize"))
end = source.index("\n/************************************************************************\n * PlaceItemNone", start)
region = source[start:end]
assert "CtrSpatialStyleIsCELLS(cw)" in region
assert "cell_width == 0 || cell_height == 0" in region
assert "return;" in region
print("PASS Container cell preferred-size checks zero metrics")
