---
nav_exclude: true
---
# Text selection conversion concatenation

`_XmTextToLocaleText` already measures every converted segment before
allocating its result. It now uses that size to copy each segment at a tracked
offset, rather than repeatedly scanning the growing result with `strcat`.
Large multi-segment selections therefore retain the same concatenated bytes
with linear copy work.

The source contract is checked by:

```sh
python3 tests/automated/check-transfer-text-concat.py
```
