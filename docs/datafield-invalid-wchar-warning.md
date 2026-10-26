---
nav_exclude: true
---
# DataField invalid wide-character warnings

Invalid wide-character conversion results can be negative, and a successful
conversion can fill the complete `MB_LEN_MAX` buffer. The warning path now
terminates its scratch buffer only when the returned length is in range;
otherwise it emits an empty diagnostic safely.

Regression coverage: `tests/automated/check-datafield-invalid-wchar-warning.py`.
