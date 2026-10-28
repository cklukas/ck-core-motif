---
nav_exclude: true
---
# ComboBox selected-position bounds

Zero-based ComboBox compatibility converts the public selected position to the
List widget's one-based position. `INT_MAX` has no representable one-based
successor, so ComboBox construction now leaves the child List unselected
instead of wrapping the position into a negative value.

Regression coverage: `tests/automated/check-combobox-selected-position-bounds.py`.
