#!/usr/bin/env python3
"""Ensure Scale label placement cannot wrap unsigned geometry differences."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text(
    encoding="utf-8"
)
for function in ("LayoutHorizontalLabels", "LayoutVerticalLabels"):
    start = source.index(f"static void \n{function}(")
    end = source.index("\nstatic void ", start + 1)
    body = source[start:end]
    assert "int first_tic_dim, last_tic_dim;" in body
    assert "/ 2.0f;" in body
    assert "Dimension first_tic_dim, last_tic_dim;" not in body
assert "sb->scrollBar.slider_area_width - \n\t\t\t   TotalWidth(w)) / 2;" not in source
assert "sb->scrollBar.slider_area_height - \n\t\t\t   TotalHeight(w)) / 2;" not in source
print("PASS Scale label placement uses signed geometry differences")
