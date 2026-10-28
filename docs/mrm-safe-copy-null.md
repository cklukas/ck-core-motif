---
nav_exclude: true
---

# Mrm callback-value null guard

`Urm__CW_SafeCopyValue` copies resource values into callback-owned storage.
The routine now rejects a null value pointer before inspecting the value or its
vector dimensions.  Valid scalar and vector callback values retain the same
copying and ownership behavior.
