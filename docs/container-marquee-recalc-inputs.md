---
nav_exclude: true
---
# Container marquee recalculation input validation

Marquee geometry recalculation now returns safely without a Container widget,
and the extend-both path requires an actual anchor child before reading its
geometry. Teardown-time selection callbacks cannot dereference missing anchor
state.

The source contract is checked by
`tests/automated/check-container-marquee-recalc-inputs.py`.
