---
nav_exclude: true
---

# DataField pixmap context allocation

`df_LoadGCs` now checks the per-screen pixmap context record before writing its
metadata or registering the destruction callback. If allocation fails, the
helper returns without dereferencing a null context record.

The focused source contract is checked by
`tests/automated/check-datafield-pixmap-context.py`.
