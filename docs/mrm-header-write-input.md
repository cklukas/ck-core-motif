---
nav_exclude: true
---
# Mrm header-writer input validation

`Idb__HDR_InitHeader` now rejects a null file handle or null creator/module
metadata before initializing the header buffer or passing those strings to
`strncat`. Valid header creation and its fixed-width truncation behavior remain
unchanged; malformed caller input returns `MrmFAILURE`.

The source-level regression is
`tests/automated/check-mrm-header-write-input.py`.
