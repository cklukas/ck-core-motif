#!/usr/bin/env python3
"""Check XmNumberBox Redisplay handles X11 resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text()
start = source.index("static void Redisplay(")
end = source.index("static void commit_text(", start)
function = source[start:end]

assert "!number->number_box.text_gc" in function
assert "!number->number_box.arrow_gc" in function
text_guard = function.index("if (!text_gc)")
font_query = function.index("font = XQueryFont")
font_guard = function.index("if (!font)")
font_use = function.index("font->ascent")
assert text_guard < font_query < font_guard < font_use
print("PASS NumberBox redisplay failure guards")
