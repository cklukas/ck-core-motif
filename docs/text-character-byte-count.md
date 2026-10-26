---
nav_exclude: true
---
# Text character-to-byte conversion counts

`_XmTextCharactersToBytes` now checks the accumulated `wctomb` byte count
before adding each converted character. An unrepresentable result stops the
conversion without wrapping the legacy integer return value.

Regression coverage: `tests/automated/check-text-character-byte-count.py`.
