---
nav_exclude: true
---
# DataField validation capacity arithmetic

DataField validation now bounds the extra storage reserved for validated
strings against both the legacy signed field and the Xt allocation width,
including the wide-character multiplication. This closes the remaining
near-limit wraparound after source-length conversion.

Regression coverage: `tests/automated/check-datafield-validate-capacity.py`.
