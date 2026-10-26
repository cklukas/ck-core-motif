---
nav_exclude: true
---

# ProgressRing state API inputs

The public ProgressRing range, value, and indeterminate-state APIs now reject
a null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid normalization,
redraw, and callback behavior are unchanged.

The focused regression check is
`tests/automated/check-progress-ring-inputs.py`.
