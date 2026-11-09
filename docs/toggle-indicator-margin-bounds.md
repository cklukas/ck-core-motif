---
nav_exclude: true
---
# ToggleButton indicator margin bounds

ToggleButton grows label margins to reserve space for its indicator. The
requested increase can exceed a legacy `Dimension`; the widget now saturates
each affected margin and records the actual increase rather than the
unrepresentable requested delta. Later layout changes can therefore remove
exactly the reserved space without wrapping an origin or margin.

The focused regression is
`tests/automated/check-toggle-indicator-margin-bounds.py`.
