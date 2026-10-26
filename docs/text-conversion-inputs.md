# Text conversion input validation

The private text-source encoding helpers now validate both buffer pointers,
the character count, and the requested character width before copying or
calling the locale conversion routines. Non-positive lengths and invalid
widths return zero without dereferencing caller-provided storage; a zero-length
byte conversion still writes its terminator only when the destination exists.

The focused source contract is checked by
`tests/automated/check-text-conversion-inputs.py`.
