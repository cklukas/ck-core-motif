#!/usr/bin/env python3
"""Check that Container cell-layout metadata allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("GetSpatialSize (")
end = source.index("/************************************************************************\n * CreateIconHeader", start)
region = source[start:end]
assert "SIZE_MAX / sizeof(XmContainerCwidCellInfoRec)" in region
assert "cwid_info == NULL" in region
assert "cw->composite.num_children != 0" in region
print("PASS Container cell-layout metadata allocation is guarded")
