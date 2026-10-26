---
nav_exclude: true
---
# XmTextFindStringWcs conversion termination

The wide-character search API now rejects a conversion that fills the entire
temporary multibyte buffer and explicitly terminates successful conversions
before passing them to the narrow search implementation.

Regression coverage: `tests/automated/check-textfind-wcs-termination.py`.
