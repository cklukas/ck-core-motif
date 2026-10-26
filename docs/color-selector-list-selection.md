---
nav_exclude: true
---
# ColorSelector list selection

Selecting a named color now treats `XmStringUnparse` as a fallible
allocation. A failed conversion leaves the current color name untouched;
successful conversions go through the same transactional resource update used
by the public color-name path.

The focused source check is
`tests/automated/check-color-selector-name-allocation.py`.
