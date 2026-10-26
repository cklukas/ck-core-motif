---
nav_exclude: true
---

# ProgressBar state API inputs

The public ProgressBar range, value, and indeterminate-state APIs now reject a
null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid range normalization,
redraw, and callback behavior are unchanged.

The focused regression check is
`tests/automated/check-progress-bar-inputs.py`.
