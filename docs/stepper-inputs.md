---
nav_exclude: true
---

# Stepper state API inputs

The public Stepper range, value, and step APIs now reject a null widget before
invoking the Xt subclass check. Invalid optional handles return the existing
neutral value or no-op result; valid normalization, redraw, and callback
behavior is unchanged.

The focused regression check is `tests/automated/check-stepper-inputs.py`.
