#!/usr/bin/env python3
"""Check that MWM colormap and menu getters reject null client records."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProperty.c").read_text(
    encoding="utf-8"
)
find_start = source.index("Colormap FindColormap")
find_end = source.index("\n} /* END OF FUNCTION FindColormap */", find_start)
find_region = source[find_start:find_end]
menu_start = source.index("GetMwmMenuItems(")
menu_end = source.index("\n} /* END OF FUNCTION GetMwmMenuItems */", menu_start)
menu_region = source[menu_start:menu_end]

assert "if (pCD == NULL)" in find_region
assert "return (colormap);" in find_region
assert "if (pCD == NULL)" in menu_region
assert "return (NULL);" in menu_region
print("PASS MWM colormap and menu getters reject null client records")
