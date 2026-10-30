---
nav_exclude: true
---

# SpinBox position hook guards

SpinBox position import/export callbacks now validate the child constraint and
output pointer before reading position state. Numeric index conversion also
requires a non-zero increment, preventing division by zero while preserving
normal position/value behavior. Invalid imports return `XmSYNTHETIC_NONE`.

The focused source-contract check is:

```sh
python3 tests/automated/check-spinbox-position-hook-guards.py
```
