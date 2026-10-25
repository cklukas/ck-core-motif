# ToggleButton pixmap centering geometry

ToggleButton centers a label pixmap vertically inside the widget. The height
difference is now computed with signed operands before the existing non-negative
clamp, so an oversized pixmap cannot wrap to a large positive vertical position
in a small button.

The focused source contract is checked by
`tests/automated/check-toggle-pixmap-center-bounds.py`.
