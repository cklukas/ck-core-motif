---
nav_exclude: true
---
# Mrm conversion allocation failures

Legacy callback and font-list conversion now checks every variable-size
allocation before dereferencing the result. Null translated callbacks and
font lists return the existing failure path, while valid UID conversion and
callback setup retain their previous behavior.

The source-level regression is
`tests/automated/check-mrm-conversion-allocation.py`.
