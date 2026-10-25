---
nav_exclude: true
---
# XmString component text pointers

`XmeStringGetComponent` now rejects optimized and unoptimized segments whose
nonzero byte count has a null text pointer. This prevents direction detection,
copying, and text iteration from dereferencing malformed component storage.

The regression check is
`tests/automated/check-xmstring-component-text.py`.
