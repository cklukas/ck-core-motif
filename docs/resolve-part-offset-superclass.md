---
nav_exclude: true
---

# Public part-offset resolution requires a superclass

`XmeResolvePartOffsets` now rejects a widget-class record whose superclass is
`NULL` before inspecting superclass sizes or class traits. A root or
partially initialized class is not a valid input for part-offset resolution;
returning with both output tables unset keeps malformed class records from
causing a null dereference while preserving the existing valid-class path.
The resolver also checks alignment rounding and the inherited-size addition
before changing either class record, so a representationally oversized class
layout fails without leaving a partially updated size.

The focused regression check is
`tests/automated/check-resolve-part-offset-allocation.py`.
