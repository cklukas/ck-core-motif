#!/usr/bin/env python3
"""Ensure SegmentedControl swaps items only after replacement buttons build."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean XmSegmentedControlSetItems")
end = source.index("\nCardinal XmSegmentedControlGetItemCount", start)
body = source[start:end]

assert "XmSegmentedControlItemRec *old_items;" in body
assert "old_items = control->segmented.items;" in body
assert body.index("old_items = control->segmented.items;") < body.index("XmCreateToggleButton")
assert body.index("XmCreateToggleButton") < body.index("XtDestroyWidget(old_items[i].button)")
assert "control->segmented.items = NULL;" not in body
assert "XtPointer selected_id = NULL;" in body
assert "copy[selected_index].id == selected_id" in body
assert "XmToggleButtonSetState(copy[i].button" in body
print("PASS SegmentedControl replacement is transactional")
