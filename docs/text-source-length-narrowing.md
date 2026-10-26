# Text source length narrowing

Text source creation now checks the multibyte limit and the byte length of a
local-text value before passing either through legacy `int` interfaces. The
wide-text `wcstombs` result remains a `size_t`, so conversion failure cannot be
confused with a wrapped positive length. Oversized inputs fail during source
creation without changing the source state.

The focused source contract is checked by
`tests/automated/check-text-source-length-narrowing.py`.
