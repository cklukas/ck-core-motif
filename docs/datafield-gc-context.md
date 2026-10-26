---
nav_exclude: true
---

# DataField GC context allocation

`df_GetTextFGCData` now checks its per-screen context record and GC-data
allocations before initialization or X context registration. A failed second
allocation releases the temporary context record and returns `NULL`, avoiding
partial registration and null dereferences.

The focused source contract is checked by
`tests/automated/check-datafield-gc-context.py`.
