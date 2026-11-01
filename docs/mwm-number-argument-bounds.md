# MWM numeric argument bounds

MWM numeric resource arguments now use explicit Boolean conversion helpers
with an output value. Decimal, hexadecimal, and octal accumulation checks for
overflow before multiplying, and a missing argument is rejected without
dereferencing a null pointer. The accepted bases and unsigned output remain
compatible with the existing parser.

The source contract is covered by
`tests/automated/check-wm-resparse-number-bounds.py`.


The numeric keysym fallback also uses the checked Boolean conversion contract:
`StrToNum` fills an unsigned output value, and `ParseKeySym` assigns that value
only after success. Its former one-argument sentinel call prevented a fresh
MWM build. `tests/automated/run-mwm-numeric-keysym.py` compiles the production
caller together with the decimal, octal and hexadecimal helpers and exercises
named symbols, valid numeric keysyms, overflow and malformed input.
