---
nav_exclude: true
---
# XmString baseline allocation failures

`_XmStringGetBaselines` now validates its output pointers and checks the line
count against Xt's `Cardinal` allocation width before allocating baseline
arrays for single-line or multi-line strings. On failure it leaves a null
baseline array and reports zero lines; successful baseline values and ownership
remain unchanged.
