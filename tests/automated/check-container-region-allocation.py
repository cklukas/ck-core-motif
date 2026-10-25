#!/usr/bin/env python3
"""Check Container removal handles XCreateRegion failure safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("RemoveItem(\n    Widget")
end = source.index("GetSpatialSize (", start)
region = source[start:end]
assert "cwid_region = XCreateRegion();" in region
assert "if (cwid_region != NULL)" in region
assert "if (cw->container.cells_region != NULL)" in region
assert "XDestroyRegion(cwid_region);" in region
print("PASS Container removal handles region allocation failure")
