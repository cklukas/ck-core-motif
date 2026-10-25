#!/usr/bin/env python3
"""Check XmStepper Redisplay handles X11 resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text()
start = source.index("static void Redisplay(")
end = source.index("static void step(", start)
function = source[start:end]

assert "!w || !s->stepper.gc" in function
font_query = function.index("font = XQueryFont")
font_guard = function.index("if (!font)")
font_use = function.index("font->ascent")
assert font_query < font_guard < font_use
print("PASS Stepper redisplay failure guards")
