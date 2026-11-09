---
nav_exclude: true
---

# RatingControl coordinate bounds

RatingControl now clamps each calculated star vertex to X11's signed 16-bit
`XPoint` range before drawing.  Wide controls remain clipped predictably
instead of wrapping individual star points.

The star-field clip rectangle now applies the same bounds before converting its
inset, width, and height to X11's 16-bit fields.

`tests/automated/check-rating-coordinate-bounds.py` protects this drawing
boundary.
