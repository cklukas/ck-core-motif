#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/Chip.c").read_text()
s = "".join(source.split())
assert "MAX(0," in s
assert "XRectangleouter=rectangle(0,0,chip->core.width,chip->core.height)" in s
assert "rounded(display,window,gc,outer,0)" in s
print("PASS chip-highlight-bounds")
