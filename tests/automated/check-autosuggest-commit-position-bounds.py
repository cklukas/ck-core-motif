#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
assert "count==1&&positions&&positions[0]>0" in s
assert "(Cardinal)(positions[0]-1)" in s
print("PASS autosuggest-commit-position-bounds")
