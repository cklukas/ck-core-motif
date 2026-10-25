---
nav_exclude: true
---

# Screen per-screen getter validation

Color calculation/allocation getters now return `NULL` for a missing screen;
bitmap conversion returns `XmMATCH_DEPTH`; and insensitive-stipple lookup
returns `None` for a missing widget or screen. Valid screen values remain
unchanged.

The focused regression check is
`tests/automated/check-screen-per-screen-getters.py`.
