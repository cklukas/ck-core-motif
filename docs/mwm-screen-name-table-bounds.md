# MWM screen-name table bounds

`InitScreenNames` now validates the signed screen count and pointer-table byte
product against both `size_t` and Xt's `Cardinal` allocation width before
allocating the table. Normal screen-name initialization is unchanged, while
invalid geometry cannot wrap into an undersized allocation.

The source contract is covered by
`tests/automated/check-mwm-screen-name-table-bounds.py`.
