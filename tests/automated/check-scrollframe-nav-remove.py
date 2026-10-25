#!/usr/bin/env python3
"""Check that ScrollFrame navigator removal validates list state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c"
text = source.read_text(encoding="utf-8")
needle = """    if (scroll_frame_data->num_nav_list != 0 &&
\t    scroll_frame_data->nav_list == NULL)
\treturn;

    for (position = 0; position<scroll_frame_data->num_nav_list; position++) {
"""
if needle not in text:
    raise SystemExit("navigator-removal list guard is missing")

print("ScrollFrame navigator-removal list guard present")
