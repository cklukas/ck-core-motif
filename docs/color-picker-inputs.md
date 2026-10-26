---
nav_exclude: true
---

# ColorPicker value API inputs

The public ColorPicker committed-value and preview APIs now reject a null
widget before invoking the Xt subclass check. Invalid optional handles return
the existing neutral value or no-op result; valid value, redraw, and callback
behavior is unchanged.

The focused regression check is
`tests/automated/check-color-picker-inputs.py`.
