# TextField multibyte conversion width

TextField selection conversion uses `wcstombs`, whose result is `size_t` (or
`(size_t)-1` on failure). The four text-target branches now retain that width
until assigning the X11 wire length, instead of storing it in `int`. This
prevents large valid conversions from being truncated or mistaken for a
conversion failure.

The contract is covered by
`tests/automated/check-textfield-wcstombs-width.py`.
