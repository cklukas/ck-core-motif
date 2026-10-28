#!/usr/bin/env python3
"""Keep accessibility selection counts within the public integer range."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Accessible.c").read_text()
start = source.index("int XmAccessibilitySelectionCount")
end = source.index("\nXmAccessibleId XmAccessibilitySelectedChild", start)
body = source[start:end]
assert "count = bounded_child_index(n->list_selected_count);" in body
assert "count = (int)n->list_selected_count" not in body
print("Accessibility selection counts are bounded before return")
