#!/usr/bin/env python3
"""Check that shared MWM frame lists include title and resize elements."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmCDecor.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean AllocateFrameDisplayLists")
end = source.index("\n}", start) + 2
region = source[start:end]

assert "frame_top_count = NUM_RESIZE_TS_ELEMENTS(pcd) +" in region
assert "NUM_TITLE_TS_ELEMENTS(pcd);" in region
assert "frame_bottom_count = NUM_RESIZE_BS_ELEMENTS(pcd) +" in region
assert "NUM_TITLE_BS_ELEMENTS(pcd);" in region
assert "frame_bottom_count = NUM_RESIZE_BS_ELEMENTS(pcd) +\n\t\t\t     NUM_RESIZE_BS_ELEMENTS(pcd);" not in region
print("PASS MWM shared frame display-list counts include title and resize elements")
