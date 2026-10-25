---
nav_exclude: true
---

# Resource integer parser overflow handling

`isInteger` now bounds-checks each digit against the signed `int` range before
accumulation, including the valid `INT_MIN` case. Overlong or out-of-range
values return `False` without signed overflow or undefined behavior.

The focused regression check is
`tests/automated/check-integer-parser-overflow.py`.
