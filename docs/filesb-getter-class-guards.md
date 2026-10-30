---
nav_exclude: true
---

# FileSelectionBox getter class guards

FileSelectionBox’s synthetic directory, pattern, label, list, count, and mask
getters now validate the widget class before accessing private state. Invalid
callback inputs keep their existing null/zero return behavior; valid
FileSelectionBox reads and child-resource ownership remain unchanged.

The focused source-contract check is
`python3 tests/automated/check-filesb-getter-class-guards.py`.
