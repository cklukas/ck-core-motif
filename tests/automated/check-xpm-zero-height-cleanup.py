#!/usr/bin/env python3
"""Check XPM empty-image rejection destroys its temporary XImage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text()
start = source.index(" * Create an XImage with its data")
start = source.index("\nCreateXImage(\n", start)
start = source.rfind("static int", 0, start)
end = source.index("\n}\n", start) + 2
function = source[start:end]

guard = "if((*image_return)->bytes_per_line == 0 ||  height == 0)"
guard_start = function.index(guard)
guard_body = function[guard_start:function.index("return XpmNoMemory;", guard_start)]
assert "XDestroyImage(*image_return);" in guard_body
assert "*image_return = NULL;" in guard_body
print("PASS XPM zero-height cleanup")
