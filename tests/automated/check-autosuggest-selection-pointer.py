#!/usr/bin/env python3
"""Check AutoSuggestBox commit handling validates selected positions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text()
start = source.index("static void search_committed(Widget widget")
end = source.index("static void list_selected(Widget", start)
body = source[start:end]

assert "count == 1 && positions != NULL && positions[0] > 0" in body
assert "count == 1 && positions[0] > 0" not in body

print("AutoSuggestBox commit handling guards missing selected positions")
