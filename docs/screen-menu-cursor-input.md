---
nav_exclude: true
---

# Screen menu-cursor input validation

Menu-cursor setters and screen-based getters now safely handle missing
displays, screens, or `XmScreen` objects. Valid cursor updates and reads are
unchanged.

The focused regression check is
`tests/automated/check-screen-menu-cursor-input.py`.
