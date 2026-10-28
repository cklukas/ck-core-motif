# Accessibility text-position bounds

Accessibility exposes text positions through signed `int` fields.  The
conversion helper now clamps both below `INT_MIN` and above `INT_MAX`, avoiding
implementation-defined narrowing for malformed or unusually large provider
positions.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-accessible-text-position-bounds.py
```
