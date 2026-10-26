---
nav_exclude: true
---

# DataField graphics-context resources

`df_LoadGCs` now handles failed `TextFGCData` lookup and failed save, text, or
image GC allocation. Earlier GCs are released when a later allocation fails,
and no context record is dereferenced when lookup cannot provide one.

The focused source contract is checked by
`tests/automated/check-datafield-gc-resources.py`.
