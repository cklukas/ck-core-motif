#!/usr/bin/env python3
"""Keep exported accessibility child counts within the protocol integer range."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/AccessibleDBus.c").read_text()
start = source.index("static int child_count")
end = source.index("\nstatic dbus_int32_t bounded_action_count", start)
body = source[start:end]
assert "size_t count = _XmAccessibilityChildCount(a, id);" in body
assert "count > (size_t)INT_MAX ? INT_MAX : (int)count" in body
print("Accessibility D-Bus child counts are bounded before export")
