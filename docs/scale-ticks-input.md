---
nav_exclude: true
---
# Scale tick-generation input guard

`XmScaleSetTicks` now returns immediately when passed a null scale widget,
before application-context lookup. Valid scales retain the existing tick
generation and range validation behavior.

The focused regression is
`tests/automated/check-scale-ticks-input.py`.
