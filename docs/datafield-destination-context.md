---
nav_exclude: true
---

# DataField destination context allocation

`df_GetTextFDestData` now checks its per-screen context and destination-data
allocations before initialization or X context registration. If the second
allocation fails, the temporary context record is released and the helper
returns `NULL`; no partially initialized context is retained.

The focused source contract is checked by
`tests/automated/check-datafield-destination-context.py`.
