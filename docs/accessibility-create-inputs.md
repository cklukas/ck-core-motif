---
nav_exclude: true
---
# Accessibility object creation inputs

`XmAccessibilityCreate` now rejects a null root and checks its own allocation
before using the new accessibility object. Invalid or allocation-failed setup
returns `NULL` without touching Xt state.
