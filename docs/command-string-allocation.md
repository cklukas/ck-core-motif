---
nav_exclude: true
---

# Command text-string allocation

`CommandCallback` now stops cleanly when retrieving the command text fails to
allocate a string. It avoids passing a null value into `XmStringGenerate` and
leaves the existing value-change callback behavior unchanged for valid text.

The focused source contract is checked by
`tests/automated/check-command-string-allocation.py`.
