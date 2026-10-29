# SpinBox validation value guard

`XmSpinBoxValidatePosition` now handles an access-text trait that returns no
string by preserving the current position and returning `XmCURRENT_VALUE`
before numeric parsing. Normal textual parsing, range validation, and position
rounding are unchanged.

The focused source contract is checked by
`tests/automated/check-spinbox-value-input.py`.
