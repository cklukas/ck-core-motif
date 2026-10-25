---
nav_exclude: true
---
# Mrm callback routine offsets

`Urm__CW_FixupCallback` now rejects callback items with a zero routine-name
offset before treating the record header as a string. Valid callbacks retain
their existing hierarchy lookup behavior.

The source-level regression is
`tests/automated/check-mrm-callback-routine-offset.py`.
