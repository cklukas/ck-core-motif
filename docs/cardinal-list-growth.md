---
nav_exclude: true
---
# Cardinal-list resource conversion bounds

The cardinal-list resource converter grows its output array while parsing
values.  Capacity doubling now checks the `int` limit and the resulting byte
count against both host `size_t` and Xt's `Cardinal` allocation range before
reallocation.  Invalid or unrepresentably large input uses the existing
cleanup path, preventing a wrapped capacity from causing an undersized array.
