---
nav_exclude: true
---

# Clipboard property-name comparison width

`GetWindowProperty` now keeps the property-name length and comparison index in
`size_t`, matching `strlen` and avoiding a narrowing conversion before the
legacy clipboard type comparison.

Regression: `tests/automated/check-clipboard-property-name-width.py`
