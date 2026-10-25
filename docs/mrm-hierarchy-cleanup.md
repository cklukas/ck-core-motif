---
nav_exclude: true
---
# Mrm hierarchy construction cleanup

`Urm__OpenHierarchy` now initializes every owned list before allocation and
runs `Urm__CloseHierarchy` when a file-list, group-list, or resource-context
allocation fails.  This releases already opened files and partial lists while
retaining the existing success and compatibility behavior.

The source-level regression is
`tests/automated/check-mrm-hierarchy-cleanup.py`.
