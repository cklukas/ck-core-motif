#!/usr/bin/env python3
"""Check SegmentedControl rejects counts its signed index API cannot represent."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
start = source.index("Boolean XmSegmentedControlSetItems")
end = source.index("\n}\n", start)
body = source[start:end]

assert "count > (Cardinal)INT_MAX" in body
assert body.index("count > (Cardinal)INT_MAX") < body.index("control =")

print("SegmentedControl rejects unrepresentable item counts")
