---
nav_exclude: true
---

# Insert-selection request guard

The insert-selection conversion callback now returns `False` when Xt cannot
provide a selection request event.  This is valid when the callback is
invoked outside an active conversion request and avoids dereferencing a null
event before the existing serial and property handling.

Regression: `tests/automated/check-insert-selection-request-guard.py`
