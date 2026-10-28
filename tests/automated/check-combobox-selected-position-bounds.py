#!/usr/bin/env python3
"""Check zero-based ComboBox position conversion rejects INT_MAX."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("static Widget\nCreateScrolledList")
end = source.index("static Boolean\nHit", start)
body = source[start:end]

assert "CBS_SelectedPosition(cb) == INT_MAX" in body
assert "leave the child unselected" in body
assert "CBS_SelectedPosition(cb) + 1" in body
print("ComboBox zero-based selected-position conversion is bounded")
