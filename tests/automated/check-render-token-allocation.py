#!/usr/bin/env python3
"""Check that render-table token string allocations terminate safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = source.index("static Token\nReadToken")
end = source.index("#ifdef\tUSE_XFT", start)
region = source[start:end]
assert region.count("new_token -> type = T_EOF;") >= 4
assert region.count("new_token -> string == NULL") >= 4
assert "*position = pos;" in region
print("PASS render-table token allocations are guarded")
