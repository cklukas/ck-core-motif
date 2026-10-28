---
nav_exclude: true
---

# UIL machine-code listing bounds

Machine-code listing records store their display-text length in a 16-bit
field, while the source text is measured with `strlen()`. The listing path now
clamps the `size_t` result to the fixed output buffer before narrowing it,
preventing a long embedded text field from wrapping and bypassing the output
limit. Machine-code vector allocation sizing is explicit as well.

Regression: `tests/automated/check-uil-machine-text-bounds.py`
