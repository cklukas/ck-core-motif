---
nav_exclude: true
---
# MRM widget-reference lookup validation

`Urm__CW_FindWRef` now rejects null outputs, invalid contexts, and null widget
names before examining the reference buffer. It clears a supplied widget output
on failure and validates descriptor counts, heap bounds, name offsets, and
terminators before comparing a stored name. Corrupt or incomplete widget
reference data therefore fails safely instead of reading outside the resource
context.

The source-level regression is
`tests/automated/check-mrm-wref-lookup-inputs.py`.
