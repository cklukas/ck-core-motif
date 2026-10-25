---
nav_exclude: true
---
# RatingControl input arithmetic

RatingControl maps a button X coordinate to the configured rating count.  The
old expression multiplied two `int` values before dividing by widget width,
so a large but valid rating count could overflow and choose the wrong value.
The intermediate product now uses `long long`; the existing `choose` clamp
continues to enforce the widget's valid range.

`tests/automated/check-rating-input-arithmetic.py` keeps the widened
intermediate in the pointer-event path.
