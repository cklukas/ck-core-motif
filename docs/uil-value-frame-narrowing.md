---
nav_exclude: true
---

# UIL value-frame field narrowing

UIL parser frames retain several compact compatibility fields (`b_type`,
`b_flags`, and `b_charset`). The value-construction paths now make each
narrowing conversion explicit at the field boundary. This documents the
existing ABI-sized storage without changing the values produced for valid UIL
input. Compound-string component and direction values likewise narrow only at
the Motif API boundary, after UIL enum validation. This keeps strict compiler
diagnostics useful for new conversions. Compound-string lengths are checked
before entering the ABI-sized `w_length` field so oversized values cannot
silently wrap. Name-binding masks are widened explicitly before combining with
the unsigned object flags field.

Font-table growth is likewise bounded by the 8-bit table-count field before a
new entry is linked.

Regression: `tests/automated/check-uil-value-frame-narrowing.py`
