---
nav_exclude: true
---
# WSM backdrop search-path bounds

The WSM backdrop-path setup prepends the user and administrative directories
to a resource-defined search path. It now computes the formatted length with
`snprintf`, rejects unrepresentable lengths, checks `XtMalloc`, and writes the
result with a bounded second pass. The old path is released only after the new
path is complete, preserving path order while avoiding integer-wrap and
allocation-copy failures.

`tests/automated/check-backdrop-path-bounds.py` guards this WSM-only path.
