#!/usr/bin/env python3
"""Check the private highlight segment renderer bounds X11 inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DrHiDash.c").read_text(
    encoding="utf-8"
)

for fragment in (
    "static short bounded_segment_coordinate(long long value)",
    "if (value < SHRT_MIN)",
    "if (value > SHRT_MAX)",
    "if (thickness > draw_width) thickness = draw_width;",
    "if (thickness > draw_height) thickness = draw_height;",
    "bounded_segment_coordinate((long long)x + draw_width - thickness)",
    "bounded_segment_coordinate((long long)y + draw_height - half_hl)",
    "bounded_line_width(gcvalues.line_width)",
):
    assert fragment in source, fragment

print("Private highlight segments clamp thickness and X11 coordinates")
