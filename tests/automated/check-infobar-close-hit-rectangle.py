#!/usr/bin/env python3
"""Check InfoBar close input matches the drawn 18x18 rectangle."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events")
end = source.index("\nstatic Boolean SetValues", start)
region = source[start:end]

assert "int close_y=(int)bar->core.height>=18?((int)bar->core.height-18)/2:0" in region
assert "event->xbutton.x<close_x+18" in region
assert "event->xbutton.y>=close_y" in region
assert "event->xbutton.y<close_y+18" in region

print("PASS InfoBar close input is limited to its drawn rectangle")
