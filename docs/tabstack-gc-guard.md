---
nav_exclude: true
---
# TabStack graphics-context guard

TabStack creates a private graphics context after realization and uses it from
resize, redisplay, and tab-selection rendering.  Those paths now skip visual
operations when GC creation fails, while preserving widget layout and
selection state.  This avoids issuing X drawing requests with a null GC under
server/resource exhaustion.
