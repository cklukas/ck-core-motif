#!/usr/bin/env python3
"""Check that Container first-column geometry cannot wrap Dimension."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("static Dimension\nAddDimensionSaturated")
end = source.index("/************************************************************************\n* SetDynamicTabList", start)
helper = source[start:end]
if "max_dimension = (uint64_t)((Dimension)-1)" not in helper:
    raise SystemExit("Container lacks a Dimension-width saturation limit")
if "return (Dimension)-1" not in helper:
    raise SystemExit("Container does not clamp overflowing first-column geometry")

start = source.index("GetDynFirstColWidth")
end = source.index("/************************************************************************\n * ResizeIconWidths", start)
region = source[start:end]
for needle in (
    "detail_indent = (uint64_t)c->depth",
    "AddDimensionSaturated(cwid_fcw",
):
    if needle not in region:
        raise SystemExit(f"Container first-column calculation is missing: {needle}")
if "cwid_fcw += cw->container.ob_width" in region:
    raise SystemExit("Container first-column calculation still wraps directly")

print("Container first-column geometry uses saturating Dimension arithmetic")
