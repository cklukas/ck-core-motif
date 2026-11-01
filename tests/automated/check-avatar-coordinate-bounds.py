#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/Avatar.c").read_text()
s = "".join(source.split())
assert "diameter=MIN(avatar->core.width,avatar->core.height)" in s
assert "x=((int)avatar->core.width-diameter)/2" in s
assert "y=((int)avatar->core.height-diameter)/2" in s
assert "XSetRegion(display,gc,clip)" in s
print("PASS avatar-coordinate-bounds")
