---
nav_exclude: true
---
# DataField selection lengths

DataField selection conversion now rejects external byte lengths that cannot
be represented by its legacy `int` replacement-length API. Oversized values
are discarded without truncating the replacement count.

Regression coverage: `tests/automated/check-datafield-selection-length.py`.
