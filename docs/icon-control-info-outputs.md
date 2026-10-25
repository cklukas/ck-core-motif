---
nav_exclude: true
---

# `XmeGetIconControlInfo` output validation

`XmeGetIconControlInfo` now returns `False` when any output pointer is missing,
before taking the process lock or writing fallback values. Calls that provide
all three outputs retain the existing color-object and fallback behavior.

The focused regression check is
`tests/automated/check-icon-control-info-outputs.py`.
