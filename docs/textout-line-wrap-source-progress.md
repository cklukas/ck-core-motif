---
nav_exclude: true
---
# TextOut line-wrap source validation

Line-end discovery now rejects non-advancing or malformed text-source blocks
before continuing its wrapping loops. This prevents malformed source data from
causing an infinite loop or reaching width/height measurement with invalid
memory.

Regression coverage: `tests/automated/check-textout-line-wrap-source-progress.py`.
