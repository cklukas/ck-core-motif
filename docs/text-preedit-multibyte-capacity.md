---
nav_exclude: true
---
# Text preedit multibyte capacity

Wide-character preedit conversion now checks the `size_t` multiplication used
to size its multibyte buffer before calculating the capacity. Extremely large
or malformed preedit lengths therefore fail closed instead of wrapping into a
small allocation; the existing Xt `Cardinal` bound remains enforced afterward.

Regression: `tests/automated/check-text-preedit-wide-conversion.py`
