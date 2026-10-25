---
nav_exclude: true
---
# XmString component structure guards

`XmeStringGetComponent` now rejects malformed multi-entry strings with null
backing strings, lines, segments, or non-empty rendition arrays. The context is
marked failed and iteration ends before invalid fields are dereferenced.

The regression check is
`tests/automated/check-xmstring-component-structure.py`.
