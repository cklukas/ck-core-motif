---
nav_exclude: true
---

# UIL value-frame field narrowing

UIL parser frames retain several compact compatibility fields (`b_type`,
`b_flags`, and `b_charset`). The value-construction paths now make each
narrowing conversion explicit at the field boundary. This documents the
existing ABI-sized storage without changing the values produced for valid UIL
input, and keeps strict compiler diagnostics useful for new conversions.

Regression: `tests/automated/check-uil-value-frame-narrowing.py`
