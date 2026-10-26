#!/usr/bin/env python3
"""Ensure Container header transitions saturate signed positions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.index("if (!CtrLayoutIsDETAIL(ncw) && CtrLayoutIsDETAIL(ccw)")
end = source.index("\n\n\n\t/*", start)
region = source[start:end]

assert "AdjustPositionSaturated(nw->core.y" in region
assert "-(int64_t)real_icon_header->core.height" in region

margin_start = source.index("Update header margins", start)
margin_end = source.index("\n\t    XtVaSetValues", margin_start)
margin_region = source[margin_start:margin_end]
assert "AdjustPositionSaturated(0" in margin_region
assert "(int64_t)real_icon_header->core.height" in margin_region

print("Container header repositioning saturates signed positions")
