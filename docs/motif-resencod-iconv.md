# Compound-text iconv output allocation

`ConvertWithIconv` now checks the initial output allocation and performs buffer
growth through a temporary pointer. If either allocation fails, conversion
returns `NULL` after releasing the buffer instead of continuing with invalid
storage or losing the existing allocation.

The regression check is
`tests/automated/check-iconv-output-allocation.py`.
