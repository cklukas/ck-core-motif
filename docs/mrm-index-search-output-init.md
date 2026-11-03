---
nav_exclude: true
---
# MRM index-search failure outputs

`Idb__INX_FindIndex` and `Idb__INX_SearchIndex` now initialize their output
values before fallible work. When an index search fails because the file or an
index record is invalid, callers cannot reuse a stale buffer or index from an
earlier lookup. The normal `MrmSUCCESS`, `MrmINDEX_LT`, and `MrmINDEX_GT`
results retain their documented buffer/index outputs for insertion and lookup
callers.

The source-level regression is
`tests/automated/check-mrm-index-search-output-init.py`.
