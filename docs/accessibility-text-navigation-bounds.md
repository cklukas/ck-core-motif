# Accessibility text-navigation bounds

Character navigation in the D-Bus accessibility adapter now computes
`offset + direction` in `long long` before clamping to the valid text range.
Offsets at `INT_MAX` or below zero therefore clamp correctly instead of
wrapping through signed `int` arithmetic.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-accessible-text-navigation-bounds.py
```
