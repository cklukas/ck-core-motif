---
nav_exclude: true
---

# RatingControl value API inputs

The public RatingControl value getter and setter now reject a null widget
before invoking the Xt subclass check. Invalid optional handles return the
existing neutral value or no-op result; valid normalization, redraw, and
callback behavior is unchanged.

The focused regression check is
`tests/automated/check-rating-control-inputs.py`.
