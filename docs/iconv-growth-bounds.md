# Iconv output growth bounds

The UTF-8 resource converter grows its output buffer when `iconv` reports
`E2BIG`. The old code doubled the allocation without checking for `size_t`
overflow, and compared an unsigned length against zero as if it were signed.

The converter now rejects lengths that cannot fit the Xt allocator, checks each
doubling before reallocating, and frees the partial result on overflow.

`tests/automated/check-iconv-growth-bounds.py` guards these contracts.
