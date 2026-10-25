#!/usr/bin/env python3
"""Check SegmentedControl item replacement is transactional."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
start = source.index("Boolean XmSegmentedControlSetItems(")
end = source.index("\nCardinal XmSegmentedControlGetItemCount", start)
region = source[start:end]

assert "if (!copy) return False;" in region
assert "if (!copy[i].id || !copy[i].label)" in region
assert "XmStringFree(copy[j].label);" in region
assert region.index("if (!copy[i].id || !copy[i].label)") < region.index("control->segmented.rebuilding")
print("PASS SegmentedControl model replacement is transactional")
