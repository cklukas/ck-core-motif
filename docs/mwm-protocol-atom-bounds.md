# MWM protocol atom-array bounds

Per-screen protocol atom arrays now validate the signed screen count and the
`sizeof(Atom)` multiplication against both `size_t` and Xt's `Cardinal` byte
limit before allocation. This applies to WM-selection ownership and optional
WM-query atoms without changing normal screen initialization.

The source contract is covered by
`tests/automated/check-mwm-protocol-atom-bounds.py`.
