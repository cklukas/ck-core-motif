#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/InfoBar.c").read_text()
start = source.index("static void Events")
end = source.index("\nstatic Boolean SetValues", start)
region = source[start:end]
assert "close_x=(int)bar->core.width>=24" in region
assert "action_x=(int)bar->core.width>120" in region
assert "event->xbutton.x>=close_x" in region
assert "event->xbutton.x>=action_x" in region
assert "event->xbutton.x>(int)bar->core.width-120" not in region
print("InfoBar hit-zone bounds present")
