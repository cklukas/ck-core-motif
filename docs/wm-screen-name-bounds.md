---
nav_exclude: true
---
# MWM screen-name allocation bounds

`InitWmScreen` now keeps the screen-name length in `size_t` until it has been
validated against the `Cardinal` size accepted by `XtRealloc`. An unrepresentable
length is rejected before the allocation, instead of being narrowed through an
`int` and potentially requesting the wrong buffer size.

The source contract is checked by:

```sh
python3 tests/automated/check-wm-screen-name-bounds.py
```
