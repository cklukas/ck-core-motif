---
nav_exclude: true
---
# XmList Motif-drop concatenation guards

Motif-drop conversion now handles a missing drag payload, null item entries,
separator-copy failure, and compound-string concatenation failure without
dereferencing or leaking partial data. Valid payloads keep their original
order and separators.
