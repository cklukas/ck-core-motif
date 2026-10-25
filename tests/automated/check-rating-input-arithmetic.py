#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/RatingControl.c").read_text()
start = source.index("static void Events")
end = source.index("\nstatic Boolean SetValues", start)
region = source[start:end]
assert "long long scaled" in region
assert "scaled=(long long)event->xbutton.x*rating->rating.count" in region
assert "scaled/(int)rating->core.width" in region
assert "event->xbutton.x*rating->rating.count)/(int)" not in region
print("RatingControl widened input arithmetic present")
