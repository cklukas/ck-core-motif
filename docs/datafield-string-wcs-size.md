---
nav_exclude: true
---
# DataField wide-string allocation sizes

`XmDataFieldGetStringWcs` now validates the document length before computing
the wide-character allocation size and before converting it to Xt's
allocation-width type. Oversized lengths return `NULL` instead of wrapping the
allocation size.

Regression coverage: `tests/automated/check-datafield-string-wcs-size.py`.
