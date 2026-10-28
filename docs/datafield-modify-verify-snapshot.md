---
nav_exclude: true
---

# DataField modify-verify snapshot conversion

Before invoking modify-verify callbacks, a wide DataField value is copied into
a temporary multibyte snapshot. That conversion now checks `wcstombs` for
failure and for output that consumes the entire buffer, explicitly terminates
valid output, and releases the snapshot on every path. Allocation-failure
paths also start with initialized pointers, so cleanup cannot inspect
uninitialized state.

Regression coverage:

```text
python3 tests/automated/check-datafield-modify-verify-snapshot.py
```
