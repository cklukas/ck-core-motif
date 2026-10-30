---
nav_exclude: true
---

# Badge geometry-query input

`QueryGeometry` now validates the widget class and geometry reply pointer
before reading badge state or writing sizing results. Malformed negotiation
returns `XtGeometryNo`; valid Badge sizing remains unchanged.

The focused source contract is checked by
`tests/automated/check-badge-query-geometry-input.py`.
