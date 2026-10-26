---
nav_exclude: true
---

# NumberBox state API inputs

The public NumberBox range, value, step, and invalid-state APIs now reject a
null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid normalization,
formatting, redraw, and callback behavior is unchanged.

The focused regression check is
`tests/automated/check-number-box-inputs.py`.
