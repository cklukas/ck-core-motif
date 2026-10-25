#!/usr/bin/env python3
"""Check XmInfoBar Redisplay handles X11 resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text()
start = source.rindex("static void Redisplay(")
end = source.index("static void Events(", start)
function = source[start:end]

assert "if (!window || !bar->info.gc)" in function
font_query = function.index("font = XQueryFont")
font_guard = function.index("if (!font)")
font_use = function.index("font->ascent")
assert font_query < font_guard < font_use
assert "XFreeFont(display, font);" in function
print("PASS InfoBar redisplay failure guards")
