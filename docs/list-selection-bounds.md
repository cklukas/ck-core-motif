---
nav_exclude: true
---
# List selection result buffers

`XmListGetMatchPos` and `XmListGetSelectedPos` now check the number of result
integers against both `size_t` and Xt's `Cardinal` width before allocating and
copying the caller-visible arrays. Ordinary empty and selected-list behavior is
unchanged.

The internal selection builders apply the same checks before constructing
cached selected-item and selected-position arrays, including rejecting a
negative non-sentinel count.

The source-level regression is
`tests/automated/check-list-selection-internals.py`.
