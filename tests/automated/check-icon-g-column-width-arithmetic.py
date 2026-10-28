#!/usr/bin/env python3
"""Check IconG column-width normalization avoids signed/unsigned wrapping."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text(
    encoding="utf-8"
)
start = source.index("/* remove the current hor indentation */")
end = source.index("/*** leaving this routine:", start)
region = source[start:end]
assert "long long available" in region
assert "available > 0" in region
assert "(long long)container_data->first_column_width" in region
assert "(Position)container_data->first_column_width" not in region
assert "-=" in region and "(Dimension)available" in region
print("PASS IconG column-width arithmetic stays signed and bounded")
