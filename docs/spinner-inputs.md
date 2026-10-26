---
nav_exclude: true
---

# Spinner state API inputs

The public Spinner active-state and phase APIs now reject a null widget before
invoking the Xt subclass check. Invalid optional handles return the existing
neutral values or no-op result; valid timer, redraw, and callback behavior is
unchanged.

The focused regression check is `tests/automated/check-spinner-inputs.py`.
