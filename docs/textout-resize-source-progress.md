---
nav_exclude: true
---
# TextOut resize-measurement source validation

Resize-time width and height measurement now rejects non-advancing or
malformed text-source blocks before continuing. On malformed input it returns
the conservative zero measurement instead of looping or measuring invalid
memory.

Regression coverage: `tests/automated/check-textout-resize-source-progress.py`.
