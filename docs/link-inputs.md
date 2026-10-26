---
nav_exclude: true
---

# Link activation API inputs

`XmLinkActivate` now rejects a null widget before checking sensitivity or
reading the widget's destruction state. Invalid optional handles remain a
no-op; valid activation, visited-state, and callback behavior is unchanged.

The focused regression check is `tests/automated/check-link-inputs.py`.
