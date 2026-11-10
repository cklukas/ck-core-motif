---
nav_exclude: true
---

# WSM display input validation

The common WSM display-metadata lookup now rejects a null `Display` before it
interns protocol atoms.  Dependent selection, screen, configuration, and
target lookup helpers therefore return their established null or `None`
failure values instead of passing an invalid display into Xlib.

Regression: `tests/automated/check-wsm-display-inputs.py`.
