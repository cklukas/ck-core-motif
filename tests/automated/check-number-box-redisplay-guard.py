#!/usr/bin/env python3
"""Check XmNumberBox Redisplay handles X11 resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text()
start = source.index("static void Redisplay(")
end = source.index("static void commit_text(", start)
function = source[start:end]

assert "!number->number_box.text_gc" in function
assert "!number->number_box.arrow_gc" in function
assert "XGetGCValues" not in function
assert "XGContextFromGC(" not in function
assert "number->core.height / 2" in function
print("PASS NumberBox redisplay has a bounded text baseline")
