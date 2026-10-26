#!/usr/bin/env python3
"""Ensure Text repaint and highlight growth handles realloc failure safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text(
    encoding="utf-8"
)

add_start = source.index("static void \nAddRedraw")
add_end = source.index("\n/*\n * Find the highlight", add_start)
add = source[add_start:add_end]
assert "RangeRec *new_range;" in add
assert "new_range == NULL" in add
assert "tw->text.repaint.maximum = requested_maximum;" in add
assert "tw->text.repaint.range = r = new_range;" in add
assert "tw->text.repaint.number++;" in add

highlight_start = source.index("static void \nInsertHighlight")
highlight_end = source.index("\n\t\n/****************************************************************", highlight_start)
highlight = source[highlight_start:highlight_end]
assert "_XmHighlightRec *new_list;" in highlight
assert "new_list == NULL" in highlight
assert "tw->text.highlight.maximum = requested_maximum;" in highlight
assert "tw->text.highlight.list = new_list;" in highlight
assert "tw->text.highlight.number++;" in highlight

print("text dynamic repaint and highlight growth: transactional realloc")
