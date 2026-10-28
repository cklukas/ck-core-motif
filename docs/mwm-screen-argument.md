---
nav_exclude: true
---
# MWM screen-function arguments

The numeric form of MWM's `f.screen` function now uses complete, range-checked
decimal parsing. Invalid, overflowing, or trailing-text arguments are rejected
with a warning instead of being silently converted to screen zero. The named
forms `next`, `prev`, and `back` retain their existing behavior.
