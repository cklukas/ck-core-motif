#!/usr/bin/env python3
"""Check XmInfoBar Redisplay handles X11 resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text()
start = source.rindex("static void Redisplay(")
end = source.index("static void Events(", start)
function = source[start:end]

assert "if (!window || !bar->info.gc)" in function
assert "XGetGCValues" not in function
assert "XGContextFromGC(" not in function
assert "XDrawRectangle(display, window, bar->info.gc" in function
assert "button_x + 5" in function
assert "button_x + 13" in function
print("PASS InfoBar redisplay uses a framed close control")
