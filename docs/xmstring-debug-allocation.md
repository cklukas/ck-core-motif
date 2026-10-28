---
nav_exclude: true
---
# XmString debug allocation

When `_XmDEBUG_XMSTRING_MEM` is enabled, XmString storage reserves a prefix
for allocation diagnostics. The allocator now checks the size before adding
that prefix and returns `NULL` when `XtMalloc` fails, rather than performing
pointer arithmetic on a null result. The matching debug free helper also
ignores null pointers before removing the prefix. Normal and debug allocation
interfaces remain private and unchanged for callers.

The focused contract is covered by
`tests/automated/check-xmstring-debug-allocation.py`.

Compound-string rendition copying also validates its output pointer before
initializing it, so malformed internal calls fail cleanly instead of
dereferencing `NULL`.
