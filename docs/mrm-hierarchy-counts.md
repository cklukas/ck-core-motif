---
nav_exclude: true
---

# Mrm hierarchy file-list bounds

`Urm__OpenHierarchy` allocates one file-list entry for each requested UID
file, and a second vector for each resource group. The count is now checked
against both `size_t` and Xt's `Cardinal` byte limits before either allocation,
so a large or malformed file count cannot wrap the allocation argument.

The focused regression check is
`tests/automated/check-mrm-hierarchy-counts.py`.
