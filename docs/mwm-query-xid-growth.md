---
nav_exclude: true
---

# MWM query-XID buffer growth

The `_MOTIF_WM_QUERY_nn` conversion path grows a temporary XID buffer in
fixed increments. Growth now checks counter overflow, `size_t` multiplication,
and Xt's `Cardinal` allocation width before `XtRealloc`; an unrepresentable
request keeps the existing warning and leaves the current buffer intact.
