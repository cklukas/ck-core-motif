---
nav_exclude: true
---

# XPM serializer width checks

Serializer pointer-difference lengths now use explicit `unsigned int` casts
after their fixed-buffer bounds are established. Extension string lengths are
also rejected when they exceed the legacy 32-bit size fields before addition.

Regression: `tests/automated/check-xpm-serializer-widths.py`
