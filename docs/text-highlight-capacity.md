---
nav_exclude: true
---

# Text highlight capacity bounds

`AddWidget` no longer performs impossible negative or host-`SIZE_MAX` checks on
the unsigned Xt `Cardinal` highlight-capacity field. The meaningful Xt
`UINT_MAX` bound remains in place before the highlight array is resized.

Regression: `tests/automated/check-text-highlight-capacity.py`
