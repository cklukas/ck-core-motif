---
nav_exclude: true
---

# Clipboard conversion target-list cleanup

The `TARGETS` conversion path obtains its first format record before sizing and
allocating the returned atom list.  Allocation or count validation failures
now release that record before leaving the conversion callback.

Regression: `tests/automated/check-clipboard-convert-targets-cleanup.py`
