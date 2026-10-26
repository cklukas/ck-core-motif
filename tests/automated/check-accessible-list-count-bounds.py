#!/usr/bin/env python3
"""Check overflow guards in the accessibility list synchronizer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text()
start = source.index("static Boolean sync_native_list(XmAccessibility *a")
body = source[start:source.index("static XmAccessibleId focused_identity", start)]
assert "custom > SIZE_MAX / sizeof(*next)" in body
assert "count > SIZE_MAX / sizeof(*next) - custom" in body
assert "old_count > SIZE_MAX / 2" in body
assert body.index("old_count > SIZE_MAX / 2") < body.index("old_count * 2")
print("PASS accessibility list count arithmetic is bounded")
