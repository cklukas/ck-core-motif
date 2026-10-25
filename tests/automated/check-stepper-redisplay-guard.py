#!/usr/bin/env python3
"""Check XmStepper Redisplay handles X11 resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text()
start = source.index("static void Redisplay(")
end = source.index("static void step(", start)
function = source[start:end]

assert "!w || !s->stepper.gc" in function
assert "XGetGCValues" not in function
assert "XGContextFromGC(" not in function
assert "s->core.height / 2" in function
print("PASS Stepper redisplay has a bounded text baseline")
