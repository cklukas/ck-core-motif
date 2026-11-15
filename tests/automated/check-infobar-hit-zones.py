#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/InfoBar.c").read_text()
start = source.index("static void Events")
end = source.index("\nstatic Boolean SetValues", start)
region = source[start:end]
assert "close_x=bar->core.width>=24?CloseLeft(bar):0" in region
assert "width=ActionWidth(bar)" in region
assert "event->xbutton.x>=close_x" in region
assert "event->xbutton.x>=action_left" in region
assert "event->xbutton.x<action_right" in region
print("InfoBar hit-zone bounds present")
