#!/usr/bin/env python3
"""Check SetSelectedIndex reports the resulting selection accurately."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
start = source.index("Boolean XmSegmentedControlSetSelectedIndex")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "select_index((XmSegmentedControlWidget)widget, index, notify, NULL);" in body
assert "return index == ((XmSegmentedControlWidget)widget)->segmented.selected;" in body
assert "return index == -1 ||" not in body
print("PASS SetSelectedIndex reports rejected clear operations")
