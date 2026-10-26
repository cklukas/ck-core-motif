# Text modify-verify callback buffers

Text modify-verify callbacks receive owned copies of replacement data.  Both
the initial byte-buffer copy and a callback-supplied replacement now validate
their signed lengths before adding terminator space and converting to Xt's
allocation type.  Invalid lengths are rejected without an overflowing
allocation request.  The wide-character callback path applies the same bound
after multiplying by `sizeof(wchar_t)`.

The focused source contract is checked by
`tests/automated/check-text-modify-verify-bounds.py`.
