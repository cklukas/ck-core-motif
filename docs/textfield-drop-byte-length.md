---
nav_exclude: true
---
# TextField drop byte lengths

The TextField drop path now rejects a converted or external byte length larger
than `INT_MAX` before passing it to character-counting or the legacy
replacement API.  The transfer value is released and the transfer is failed
cleanly instead of truncating the length during a conversion to `int`.

Regression coverage: `tests/automated/check-textfield-drop-byte-length.py`.
