---
nav_exclude: true
---
# FontSelector font-choice allocation

The XLFD/other-font choice builder now checks argument merging, its button box,
and both toggle buttons before callback registration and child storage. Partial
construction releases merged arguments and returns safely.
