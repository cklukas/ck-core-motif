---
nav_exclude: true
---

# Data-field delete-selection request guard

The data-field `DELETE` selection conversion now returns `False` when no
active request event is available for either a primary-selection or drop
operation.  Normal requestor, timestamp, and replacement handling remains
unchanged, while out-of-context callbacks no longer dereference a null event.

Regression: `tests/automated/check-delete-selection-request-guard.py`
