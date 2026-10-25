---
nav_exclude: true
---
# List click-state guard

List selection handling now checks that the internal item table exists before
building a selection callback. A missing table is treated as a no-op, which
avoids assertion failure during widget teardown or reentrant callbacks while
leaving normal click behavior unchanged.

The regression check is `tests/automated/check-list-click-state.py`.
