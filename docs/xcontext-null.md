# XContext null initialization

`XContext` is an integer Xlib identifier. The non-DnD context owners now use
integer zero for their uninitialized state instead of casting the pointer-valued
`NULL` macro. This avoids pointer-to-integer diagnostics on 64-bit builds while
preserving the existing zero sentinel and context lifecycle.

The focused source check is
`tests/automated/check-xcontext-zero.py`. The held DragBS and DragIcon transfer
paths are intentionally outside this change.
