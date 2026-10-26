---
nav_exclude: true
---
# TextField selection export buffers

TextField selection conversion now uses one bounded allocation helper for
temporary byte strings and checks every allocation before copying, converting,
or terminating the buffer.  A length that cannot fit the toolkit allocation
type, or an allocation failure, returns `False` without dereferencing an
invalid buffer.

Regression coverage: `tests/automated/check-textfield-selection-buffers.py`.
