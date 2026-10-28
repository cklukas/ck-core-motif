# TextField wide-transfer length bound

The TextField drop path stores the converted character count in `int` and then
allocates `(count + 1) * sizeof(wchar_t)`. Counts below `INT_MAX` are already
bounded for the Xt allocation; `INT_MAX` itself is not representable after the
terminator increment. The path now rejects that boundary before performing
`+ 1`, preserving safe failure for oversized transfers.

The boundary contract is covered by
`tests/automated/check-textfield-wide-length-bound.py`.
