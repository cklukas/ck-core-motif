---
nav_exclude: true
---

# MWM WSM session-file path bounds

WSM session save and restore paths now use size-safe length checks and bounded
formatting. Oversized or unrepresentable session paths fall back through the
existing default filename behavior instead of relying on integer casts or
unbounded writes.
