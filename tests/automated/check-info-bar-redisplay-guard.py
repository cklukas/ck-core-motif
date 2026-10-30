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
assert "int button_cx = button_x + 9" in function
assert "int button_cy = button_y + 9" in function
assert "button_cx - 4, button_cy - 4" in function
assert "button_cx + 4, button_cy + 4" in function
assert "button_cx + 4, button_cy - 4" in function
assert "button_cx - 4, button_cy + 4" in function
print("PASS InfoBar redisplay uses a framed close control")
