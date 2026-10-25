#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmIm.c").read_text()
start = source.index("static Cardinal\nadd_ref")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "Cardinal increment = refs->max_refs / 2;" in body
assert "if (increment == 0)" in body
assert "(Cardinal)-1 - increment" in body
assert "SIZE_MAX / sizeof(*new_refs)" in body
assert "UINT_MAX / sizeof(*new_refs)" in body
assert "UINT_MAX / sizeof(*new_callbacks)" in body

shrink_start = source.index("/* Free some storage from the array? */")
shrink_end = source.index("\n\t}", shrink_start) + 3
shrink = source[shrink_start:shrink_end]
assert "SIZE_MAX / sizeof(Widget)" in shrink
assert "UINT_MAX / sizeof(Widget)" in shrink
assert "UINT_MAX / sizeof(XIMCallback *)" in shrink

print("XmIm reference-table growth arithmetic is bounded")
