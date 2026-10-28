---
nav_exclude: true
---
# Unsupported Text LINK operations

Text and TextField do not implement internal `XmLINK` destination operations.
Those callbacks now explicitly complete such transfers with failure instead of
returning while the transfer remains pending.

Regression coverage: `tests/automated/check-text-link-operation.py`.
