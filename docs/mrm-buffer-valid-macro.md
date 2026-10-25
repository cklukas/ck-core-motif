---
nav_exclude: true
---
# Mrm buffer-validity predicate

The shared `Idb__BM_Valid` predicate now checks for a null buffer before
reading its validation field.  Marking and decommitting APIs can therefore
reject missing handles safely instead of crashing in the validation macro.

The source-level regression is
`tests/automated/check-mrm-buffer-valid-macro.py`.
