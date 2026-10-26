# Text reset overstrike allocation

Input-method reset now checks the overstrike cursor-buffer size and allocation
before converting the current multibyte value into native text units. Failure
restores the insertion-point display and resets the preedit state without
passing null storage to the conversion helper.

The focused source contract is checked by
`tests/automated/check-text-reset-overstrike-allocation.py`.
