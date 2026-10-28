---
nav_exclude: true
---
# ComboBox selected-position bounds

Zero-based ComboBox compatibility converts the public selected position to the
List widget's one-based position. `INT_MAX` has no representable one-based
successor, and negative values other than `XmINVALID_POSITION` are not valid
positions. ComboBox construction now leaves the child List unselected for
those inputs instead of wrapping or indexing with an invalid position.

Regression coverage: `tests/automated/check-combobox-selected-position-bounds.py`.
