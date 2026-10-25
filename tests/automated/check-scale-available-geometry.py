#!/usr/bin/env python3
"""Check Scale trough available-size arithmetic cannot wrap."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text(encoding="utf-8")
assert source.count("int avail;") >= 2
assert "avail = (int)XtWidth(sw) - (int)lp - (int)tp;" in source
assert "avail = (int)XtHeight(sw) - (int)lp - (int)tp;" in source
assert "scrollBox.width = (Dimension)avail;" in source
assert "scrollBox.height = (Dimension)avail;" in source
assert "avail = XtWidth(sw) - lp - tp;" not in source
assert "avail = XtHeight(sw) - lp - tp;" not in source
assert "scrollBox.x = (XtWidth(sw) - sb_min) / 2;" not in source
assert "scrollBox.y = (XtHeight(sw) - sb_min) / 2;" not in source
print("scale available trough geometry uses signed arithmetic")
