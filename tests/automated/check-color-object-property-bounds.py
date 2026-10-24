#!/usr/bin/env python3
"""Check bounded ColorObj property serialization."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(encoding="utf-8")
start = text.index("\nFetchPixelData(Widget w")
end = text.index("static Boolean", start)
region = text[start:end]
assert region.count("snprintf(tmp, sizeof(tmp)") == 2
assert 'sprintf(tmp, "%x_"' not in region
assert 'sprintf(tmp,"%lx_' not in region
print("ColorObj property serialization is bounded")
