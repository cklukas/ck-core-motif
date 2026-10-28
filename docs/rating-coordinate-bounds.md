---
nav_exclude: true
---

# RatingControl coordinate bounds

RatingControl now clamps each calculated star vertex to X11's signed 16-bit
`XPoint` range before drawing.  Wide controls remain clipped predictably
instead of wrapping individual star points.

`tests/automated/check-rating-coordinate-bounds.py` protects this drawing
boundary.
