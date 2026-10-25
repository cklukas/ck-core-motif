---
nav_exclude: true
---

# WSM window-data lookup

`_WSMGetMatchingWinData` now returns no match when the supplied data list is
null or empty.  This avoids pointer traversal on an invalid list while
preserving normal name lookup behavior.
