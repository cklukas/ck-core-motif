---
nav_exclude: true
---
# FontSelector current-font retrieval

The `XmNcurrentFont` get-values hook now handles partial initialization. If
font information, the current font record, or the caller's output pointer is
unavailable, the query returns without dereferencing incomplete widget state.
The XLFD output buffer is required only when XLFD mode is active; family-mode
queries remain available without it.
