#!/usr/bin/env python3
"""Check Container spatial-size adjustment validates pointer and count inputs."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("GetSpatialSizeCellAdjustment(\n")
region = source[start : source.index("\n}\n", start)]
guard = "if (wid == NULL || parm_width_in_cells == NULL"
assert region.index(guard) < region.index("width_in_cells = *parm_width_in_cells")
assert "cwid_info_count < 0" in region
assert "cwid_info == NULL && cwid_info_count > 0" in region

print("PASS Container spatial-size adjustment validates pointer and count inputs")
