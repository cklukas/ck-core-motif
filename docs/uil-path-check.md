---
nav_exclude: true
---

Parameterize the UIL path regression
====================================

`check-uil-paths.py` now performs a clean static-only skip when a generic
no-argument sweep does not provide a UIL binary and results directory.  With
both absolute paths supplied, it retains the existing strict runtime test for
overlong include paths.
