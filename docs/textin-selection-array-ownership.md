---
nav_exclude: true
---
# TextIn selection-array ownership

The input method keeps a static built-in selection array when a caller has not
provided one or an allocation fails.  `InputSetValues` now frees the previous
array only when it is dynamically owned, avoiding an invalid free of the static
fallback during resource updates.

Regression coverage: `tests/automated/check-textin-selection-array-ownership.py`.
