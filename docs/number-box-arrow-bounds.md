---
nav_exclude: true
---

# NumberBox arrow coordinate bounds

NumberBox now clamps its up/down arrow vertices before converting them to
X11's signed 16-bit `XPoint` fields.  Tall or explicitly unusual controls no
longer wrap triangle vertices to unrelated coordinates.

`tests/automated/check-number-box-arrow-bounds.py` protects this drawing
boundary.
