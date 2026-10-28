---
nav_exclude: true
---

# Text source value cleanup

When `_XmStringSourceGetValue` cannot read the complete source range, the
byte-string branch now frees its partially filled temporary buffer before
returning failure.  Successful retrieval and wide-character handling are
unchanged.

Regression: `tests/automated/check-text-source-value-cleanup.py`
