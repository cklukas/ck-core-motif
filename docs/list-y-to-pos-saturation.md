---
nav_exclude: true
---
# List Y-to-position saturation

`XmListYToPos` uses an out-of-range position marker below the list. The marker
now saturates at `INT_MAX` for a maximum-sized List, and the public conversion
does not increment that saturated value. This preserves normal positions while
avoiding signed overflow on extreme list sizes.

Regression coverage: `tests/automated/check-list-y-to-pos-saturation.py`.
