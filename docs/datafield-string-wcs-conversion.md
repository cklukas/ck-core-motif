---
nav_exclude: true
---
# DataField wide-string conversion failures

`XmDataFieldGetStringWcs` now returns a valid empty wide string when the
current multibyte value cannot be converted in the active locale. This avoids
returning an unterminated or partially initialized buffer after `mbstowcs`
reports an encoding error.

Regression coverage: `tests/automated/check-datafield-string-wcs-conversion.py`.
