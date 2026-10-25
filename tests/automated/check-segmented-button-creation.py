#!/usr/bin/env python3
"""Check SegmentedControl handles toggle-button creation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/SegmentedControl.c"
text = source.read_text(encoding="utf-8")

start = text.index("Boolean XmSegmentedControlSetItems")
body = text[start:text.index("Cardinal XmSegmentedControlGetItemCount", start)]
assert "if (!copy[i].button)" in body
assert "XtWarning" in body
assert "XtDestroyWidget(copy[j].button)" in body
assert "control->segmented.item_count = 0;" in body
assert "return False;" in body
print("SegmentedControl button creation failures are cleaned up")
