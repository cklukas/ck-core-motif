# Scaled-image print resolution bounds

The optional Xprint fast path derives an integer image resolution from the
source-to-destination scale ratio.  A tiny ratio can produce a value larger
than `INT_MAX`; the renderer now verifies finiteness and range before narrowing
to the Xprint API's `int` parameter.  Out-of-range values use the normal
scaled-image path instead of invoking the extension with an invalid value.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-image-print-resolution-bounds.py
```
