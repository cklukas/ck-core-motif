---
nav_exclude: true
---
# Clipboard target buffers

Clipboard target converters must return storage when reporting a non-empty
`XA_ATOM` list. Both ordinary and deferred target paths now reject a missing
buffer before casting or iterating it, and close the active copy cleanly.

The focused regression is
`tests/automated/check-clipboard-target-buffer.py`.
