---
nav_exclude: true
---
# Mrm index split bounds

Index splits now reject impossible record counts and malformed pivot strings
before hoisting an entry.  Failed left-record allocation is checked before the
new buffer is used.  Empty right-hand partitions produced by a valid two-entry
split are handled explicitly, preserving normal B-tree behavior while keeping
malformed ranges inert.

The source-level regression is
`tests/automated/check-mrm-index-split-bounds.py`.
