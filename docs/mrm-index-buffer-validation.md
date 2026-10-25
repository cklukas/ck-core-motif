---
nav_exclude: true
---
# Mrm index buffer validation

Index insertion and child-repair helpers now reject missing record storage and
invalid parent identifiers before dereferencing buffers.  The local record
validation predicates also handle null buffers, allowing split and repair
paths to report a bad record instead of crashing.

The source-level regression is
`tests/automated/check-mrm-index-buffer-validation.py`.
