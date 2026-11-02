---
nav_exclude: true
---
# Virtual-binding converter bounds

The virtual-key resource converter parses a binding string into an array of
`XmKeyBindingRec` values. It now validates the parsed count against both native
`size_t` and Xt's `Cardinal` allocation ranges before it derives the output
byte count. Oversized input fails as a normal resource conversion failure,
rather than wrapping the result size before allocation or copying.

The focused regression check is
`tests/automated/check-virtual-binding-converter-bounds.py`.
