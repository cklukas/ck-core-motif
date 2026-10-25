---
nav_exclude: true
---
# TextField clear-selection bounds

`clear-selection` now validates the selected span before converting it to the
existing `int` insertion-length contract.  This prevents `span + 1` from
wrapping when a malformed or externally supplied selection range is extremely
large, and protects the wide-character replacement allocation before its
element-size multiplication.  Ordinary selections keep the same replacement,
callback, and cursor behavior.

The TextField source build is compiled with warnings enabled; the previous
out-of-bounds warning in `ClearSelection` is no longer emitted.
