---
nav_exclude: true
---
# Text primary-selection completion widgets

Text and TextField primary-selection completion callbacks now validate the
callback widget before deriving widget state. A completion arriving during
widget teardown therefore returns safely instead of dereferencing a null
widget.

Regression coverage: `tests/automated/check-text-primary-selection-done-widget.py`.
