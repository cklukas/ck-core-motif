---
nav_exclude: true
---
# TextField TARGETS allocation

The TextField TARGETS selection target now checks the result of
`XmeStandardTargets()` before appending widget-specific atoms.  If the target
list cannot be allocated, conversion returns `False` rather than writing
through a null pointer.

Regression coverage: `tests/automated/check-textfield-targets-allocation.py`.
