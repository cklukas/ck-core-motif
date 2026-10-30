---
nav_exclude: true
---

# Avatar geometry-query input

`QueryGeometry` now validates the widget class and geometry reply pointer
before writing the requested size. Malformed geometry negotiation returns
`XtGeometryNo`; valid Avatar sizing remains unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-query-geometry-input.py`.
