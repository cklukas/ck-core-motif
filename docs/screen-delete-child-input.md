---
nav_exclude: true
---

# Screen child deletion input validation

Screen child deletion now returns safely for a missing widget, parent, or
backing child list. Valid child removal and list compaction remain unchanged.

The focused regression check is
`tests/automated/check-screen-delete-child-input.py`.
