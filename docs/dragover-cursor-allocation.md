---
nav_exclude: true
---
# Drag-over cursor allocation

Drag-over cursor creation now rejects an X cursor-creation failure and returns
`None` so the existing drag-mode fallback can run.  When caching is enabled,
the cache record allocation is checked before initialization; if it fails, the
new cursor is released and the same fallback is used instead of dereferencing
a null cache record.

The focused source-level regression is
`tests/automated/check-dragover-cursor-allocation.py`.
