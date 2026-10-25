---
nav_exclude: true
---
# Scale value-label formatting

Scale computes the trough, ascent, descent, and label dimensions by formatting
the configured minimum and maximum values into a small temporary buffer. The
four non-Xft layout paths now use `snprintf` with the buffer size for both
integer-only and decimal-label forms. This keeps the existing label metrics and
resource behavior while making the buffer contract explicit if value ranges or
locale formatting change.

`tests/automated/check-scale-label-bounds.py` verifies that all four paths use
bounded formatting and that the former unbounded calls are absent.
