---
nav_exclude: true
---

# List top-position import guards

The List synthetic top-position converter now validates the widget class and
rejects `INT_MIN` before converting the public one-based position to Motif’s
internal zero-based value. This prevents signed underflow while preserving the
existing conversion for valid inputs.

The focused source-contract check is
`python3 tests/automated/check-list-position-import-guards.py`.
