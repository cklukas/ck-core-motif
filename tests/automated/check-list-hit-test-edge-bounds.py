#!/usr/bin/env python3
"""Check List hit testing widens the top content edge before subtraction."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("WhichItem(XmListWidget w,")
end = source.index("XmListYToPos(Widget w,", start)
region = source[start:end]

assert "int EventY" in region
assert "int y = EventY;" in region
assert "(long long)y <= (long long)lw->list.BaseY -" in region
assert "(Position)(lw->list.BaseY - lw->list.HighlightThickness)" not in region
assert "if (y > (int)lw->core.height)" in region
assert "(long long)lw->list.top_position + lines" in region
horizontal_start = source.rindex("XmListSetHorizPos(")
horizontal = source[horizontal_start:source.index("\n}\n", horizontal_start) + 3]
assert "lw->list.XOrigin = bounded_position(position);" in horizontal
print("PASS List hit testing widens the top edge before subtraction")
