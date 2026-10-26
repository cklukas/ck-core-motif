---
nav_exclude: true
---
# DataField selection-value allocation bounds

Text, locale, and compound-text selection responses allocate a temporary
buffer before copying or converting the selected value.  Those paths now share
a checked helper that rejects lengths outside the `Cardinal` range and returns
cleanly on allocation failure.  This prevents a truncated allocation or a
null dereference while preserving the existing selection formats and ABI.

The focused regression is
`tests/automated/check-datafield-selection-value-allocation.py`.
