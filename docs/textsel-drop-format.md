---
nav_exclude: true
---
# Text selection drop format validation

Text drops accepted by `DropTransferProc` are byte-oriented targets. The
callback now rejects external 16-bit and 32-bit formats, and lengths that
cannot be represented by the text conversion path, before conversion or
insertion.

Regression coverage: `tests/automated/check-textsel-drop-format.py`.
