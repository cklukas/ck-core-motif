---
nav_exclude: true
---
# Mrm callback routine-string bounds

`Urm__CW_FixupCallback` now validates each serialized callback routine offset
before converting it to a string.  The offset must point beyond the widget
record header, stay within the record, and reach a NUL terminator before the
hierarchy lookup runs.  Valid callback registration and invocation remain
unchanged.

The source-level regression is
`tests/automated/check-mrm-callback-routine-string-bounds.py`.
