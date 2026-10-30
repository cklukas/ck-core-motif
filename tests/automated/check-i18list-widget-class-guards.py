#!/usr/bin/env python3
"""Check exported I18List convenience APIs validate their widget class."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(encoding="utf-8")
for name in ("Xm18IListUnselectAllItems", "Xm18IListUnselectItem", "XmI18ListToggleRow"):
    start = source.index(name)
    region = source[start:source.index("\n}", start)]
    assert "XtIsSubclass(w, xmI18ListWidgetClass)" in region, name
    guard = region.index("XtIsSubclass(w, xmI18ListWidgetClass)")
    dereference = min(
        index for token in ("XmI18List_row_data", "ToggleRow(w")
        for index in [region.find(token)]
        if index >= 0
    )
    assert guard < dereference, name
toggle = source[source.index("XmI18ListToggleRow"):source.index("\n}", source.index("XmI18ListToggleRow"))]
assert "row < 0" in toggle
assert "row >= XmI18List_num_rows" in toggle
assert toggle.index("row < 0") < toggle.index("ToggleRow(w")
print("PASS I18List convenience APIs validate widget class before row access")
