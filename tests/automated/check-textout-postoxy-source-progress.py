#!/usr/bin/env python3
"""Check TextOut position-to-coordinate reads validate source progress."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
start = source.index("static Boolean \nPosToXY")
end = source.index("\n  local_x -= data->hoffset;", start)
body = source[start:end]

assert "XmTextPosition next =" in body
assert "next <= linestart" in body
assert "block.length > 0 && block.ptr == NULL" in body
assert "return False;" in body

print("TextOut position-to-coordinate reads guard source progress")

visible_start = source.index("\nMakePositionVisible(")
visible_end = source.index("\n}\n\n", visible_start)
visible = source[visible_start:visible_end]
assert "line_start = line_num ? tw->text.line_table[line_num].start_pos : 0;" in visible
assert "target_position = position > (XmTextPosition)(data->rows / 2)" in visible
assert "(int) (tw->text.line_table[line_num].start_pos)" not in visible
