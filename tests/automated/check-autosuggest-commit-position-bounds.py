#!/usr/bin/env python3
"""Check AutoSuggestBox committed selections reject non-positive positions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text()
start = source.rindex("static void search_committed")
body = source[start : source.index("static void list_selected", start)]
assert "count == 1 && positions[0] > 0" in body
assert "(Cardinal)(positions[0] - 1)" in body
print("PASS AutoSuggestBox committed positions are validated")
