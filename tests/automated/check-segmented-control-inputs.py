#!/usr/bin/env python3
"""Check SegmentedControl public APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text(encoding="utf-8")
set_items = source[source.index("Boolean XmSegmentedControlSetItems") : source.index("Cardinal XmSegmentedControlGetItemCount")]
get_count = source[source.index("Cardinal XmSegmentedControlGetItemCount") : source.index("int XmSegmentedControlGetSelectedIndex")]
get_selected = source[source.index("int XmSegmentedControlGetSelectedIndex") : source.index("const char *XmSegmentedControlGetItemId")]
get_id = source[source.index("const char *XmSegmentedControlGetItemId") : source.index("Boolean XmSegmentedControlSetSelectedIndex")]
set_selected = source[source.index("Boolean XmSegmentedControlSetSelectedIndex") : source.index("void XmSegmentedControlClearSelection")]
assert "widget == NULL ||" in set_items
assert "widget != NULL && XmIsSegmentedControl(widget)" in get_count
assert "widget != NULL && XmIsSegmentedControl(widget)" in get_selected
assert "widget == NULL || !XmIsSegmentedControl(widget)" in get_id
assert "widget == NULL || !XmIsSegmentedControl(widget)" in set_selected
print("PASS SegmentedControl public APIs validate null widgets")
