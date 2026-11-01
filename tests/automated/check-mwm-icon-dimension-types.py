#!/usr/bin/env python3
"""Check MWM converts icon geometry at its signed/Xlib boundary."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmImage.c").read_text(
    encoding="utf-8"
)
start = source.index("Pixmap MakeIconPixmap")
end = source.index("} /* END OF FUNCTION MakeIconPixmap */", start)
region = source[start:end]

assert "unsigned int min_width, min_height, max_width, max_height;" in region
assert "pSD->iconImageMaximum.width <= 0" in region
assert "(width - max_width) / 2U > (unsigned int)INT_MAX" in region
assert "(height - max_height) / 2U > (unsigned int)INT_MAX" in region
assert "depth == (unsigned int)display_depth" in region
assert "XtGetValues (menuW, (ArgList)args, (Cardinal)i);" in source
print("PASS MWM icon composition validates signed and Xlib geometry boundaries")
