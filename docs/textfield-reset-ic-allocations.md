---
nav_exclude: true
---
# TextField input-method reset allocations

Resetting an active preedit now handles allocation failures and invalid
multibyte conversions without dereferencing null buffers or indexing with a
negative conversion result.  The input-method state and insertion-point
painting are restored on each failure path. The multibyte length remains a
`size_t` until it has been checked against the preedit limit before narrowing
to the legacy `int` insertion count.

Regression coverage: `tests/automated/check-textfield-reset-ic-allocations.py`.
