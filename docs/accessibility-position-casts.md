# Accessibility position casts

Accessibility selection ranges and visual-line offsets now use the shared
two-sided `XmTextPosition` clamp before filling their signed `int` outputs.
This keeps large or malformed provider positions from bypassing the boundary
guard and wrapping in the public accessibility API.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-accessible-position-casts.py
```
