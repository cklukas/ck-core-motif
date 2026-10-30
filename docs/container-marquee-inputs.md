---
nav_exclude: true
---
# Container marquee input validation

Marquee selection now skips null child nodes and missing constraints, while
`InMarquee` returns safely when a child has no Container parent. Valid marquee
selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-marquee-inputs.py`.
