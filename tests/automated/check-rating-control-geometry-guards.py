#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/RatingControl.c").read_text()
s = "".join(source.split())
assert "!window||!rating->rating.gc" in s
assert "width>0&&height>0&&rating->rating.count<=width" in s
assert "XSetClipRectangles" in s
assert "if(diameter<2)continue;" in s
print("PASS rating-control-geometry-guards")
