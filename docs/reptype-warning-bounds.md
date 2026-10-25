---
nav_exclude: true
---
# RepType conversion warning bounds

When a representation-type conversion receives an unknown value, Motif builds
a warning from the localized message and the signed integer value. The old
allocation added a fixed ten bytes for the number, which could be too small for
the sign, all digits, separator, and terminator.

The warning is now sized with `snprintf` before allocation, preserving the full
diagnostic for every representable integer.

`tests/automated/check-reptype-warning-bounds.py` guards the formatting path.
