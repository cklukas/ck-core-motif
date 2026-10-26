---
nav_exclude: true
---
# Text wide search conversion

`XmTextFindStringWcs` now retains the `wcstombs` result as `size_t` until it
has validated the conversion and legacy search-length limit. This avoids
truncating a large converted byte count before the ordinary search guards run.

Regression coverage: `tests/automated/check-text-find-wcs-conversion.py`.
