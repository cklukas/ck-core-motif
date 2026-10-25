#!/usr/bin/env python3
"""Check List selection callbacks handle temporary array allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("cb.selected_items =", source.index("DefaultAction"))
end = source.index("/* BEGIN OSF Fix CR 4576 */", start)
region = source[start:end]
assert "cb.selected_item_positions =" in region
assert "if (cb.selected_items == NULL || cb.selected_item_positions == NULL)" in region
assert region.index("if (cb.selected_items == NULL") < region.index("cb.selected_items[i]")
assert "DEALLOCATE_LOCAL((char *)cb.selected_items);" in region
assert "DEALLOCATE_LOCAL((char *)cb.selected_item_positions);" in region

click_start = source.index("cb.selected_items =", source.index("ClickElement"))
click_end = source.index("/* BEGIN OSF Fix CR 4576 */", click_start)
click_region = source[click_start:click_end]
assert "if (cb.selected_items == NULL || cb.selected_item_positions == NULL)" in click_region
print("PASS List selection callbacks handle temporary allocation failure")
