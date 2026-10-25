#!/usr/bin/env python3
"""Check that XmListDeleteAllItems validates its widget input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.rindex("XmListDeleteAllItems(")
end = source.index("static void\nAPIReplaceItems", start)
region = source[start:end]
guard = region.index("if (w == NULL)")
assert region.index("_XmWidgetToAppContext(w)") > guard
assert region.index("lw->list.items") > guard
print("PASS list delete-all validates widget input")
