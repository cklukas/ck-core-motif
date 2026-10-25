---
nav_exclude: true
---

# String-to-entity converter input validation

The widget/window name converter now returns `False` for missing or malformed
Xt argument, source, destination, or widget context instead of dereferencing
them. Valid `self` and descendant-name conversions retain their existing
behavior.

The focused regression check is
`tests/automated/check-string-entity-inputs.py`.
