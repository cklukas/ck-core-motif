---
nav_exclude: true
---
# XmString rendition entry validation

`XmeStringGetComponent` now rejects null individual rendition tags even when
the surrounding rendition arrays are present. Iteration ends cleanly before
copying, measuring, or updating active rendition state.

The regression check is
`tests/automated/check-xmstring-rendition-entry.py`.
