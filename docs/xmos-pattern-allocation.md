---
nav_exclude: true
---
# Xmos pattern allocation

`GetFixedMatchPattern` now checks its output-buffer allocation before writing
the regular-expression prefix and converted pattern. Allocation failure
returns `NULL`, matching its existing invalid-input behavior without changing
successful pattern conversion.

Multibyte characters are copied with their validated `mblen` byte count via
`memcpy`, avoiding a signed-length conversion in the bounded copy path.
