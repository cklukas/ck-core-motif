---
nav_exclude: true
---

# Clipboard retrieval-size overflow guard

`ClipboardRetrieveItem` extends a serialized clipboard property by a caller
requested number of bytes. It now rejects negative extensions and arithmetic
overflow before allocation, preventing a wrapped size from causing an
undersized buffer followed by an oversized copy. Valid retrievals retain their
existing allocation and copy behavior.

The focused regression check is
`tests/automated/check-clipboard-retrieve-size-overflow.py`.
