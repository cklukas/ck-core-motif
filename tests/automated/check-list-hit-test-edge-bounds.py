#!/usr/bin/env python3
"""Check List hit testing widens the top content edge before subtraction."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("WhichItem(XmListWidget w,")
end = source.index("XmListYToPos(Widget w,", start)
region = source[start:end]

assert "(long long)y <= (long long)lw->list.BaseY -" in region
assert "(Position)(lw->list.BaseY - lw->list.HighlightThickness)" not in region
print("PASS List hit testing widens the top edge before subtraction")
