# MWM numeric argument bounds

MWM numeric resource arguments now use explicit Boolean conversion helpers
with an output value. Decimal, hexadecimal, and octal accumulation checks for
overflow before multiplying, and a missing argument is rejected without
dereferencing a null pointer. The accepted bases and unsigned output remain
compatible with the existing parser.

The source contract is covered by
`tests/automated/check-wm-resparse-number-bounds.py`.
