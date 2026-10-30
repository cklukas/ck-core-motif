---
nav_exclude: true
---
# Container marquee drawing input validation

`DrawMarquee` now returns safely when called without a widget before checking
realization or reading Container state. Valid realized marquee drawing is
unchanged.

The source contract is checked by
`tests/automated/check-container-draw-marquee-input.py`.
