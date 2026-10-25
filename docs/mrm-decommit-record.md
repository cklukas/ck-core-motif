---
nav_exclude: true
---
# Mrm buffer decommit record validation

A modified write buffer must retain both its owning file and its record storage
until decommit completes. `Idb__BM_Decommit` now rejects a missing record before
calling the low-level writer, preventing malformed buffer state from reaching a
null dereference while preserving the normal write/decommit path.
