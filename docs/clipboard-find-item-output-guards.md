---
nav_exclude: true
---

# Clipboard item lookup output guards

`ClipboardFindItem` now initializes its result pointers, rejects null output
arguments, and stops when an item ID cannot be converted to an X11 atom. This
prevents invalid IDs from reaching property lookup and gives callers
deterministic empty outputs on failure; valid lookups are unchanged.

The focused regression check is
`tests/automated/check-clipboard-find-item-output-guards.py`.
