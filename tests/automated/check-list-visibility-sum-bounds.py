#!/usr/bin/env python3
"""Check List visibility sums use wide arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
assert source.count("(long long)lw->list.top_position +") >= 2
assert source.count("(long long)newlw->list.top_position +") >= 2
draw_item = source[source.index("DrawItem(Widget w,"):source.index("DrawHighlight(XmListWidget lw,")]
draw_highlight = source[source.index("DrawHighlight(XmListWidget lw,"):source.index("static void DrawList")]
assert "position >= (lw->list.top_position + lw->list.visibleItemCount)" not in draw_item
assert "position >= (lw->list.top_position + lw->list.visibleItemCount)" not in draw_highlight
set_values = source[source.index("SetValues(Widget old,"):source.index("static void Destroy")]
assert "newlw->list.top_position + newlw->list.visibleItemCount" not in set_values
print("List visibility sums use wide arithmetic")
