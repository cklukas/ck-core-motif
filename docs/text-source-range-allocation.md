# Text source range allocation

`_XmStringSourceGetString` converts a requested text range into either native
bytes or `wchar_t` elements.  The range helper now sizes the terminator in the
same units as the requested element type, so wide strings receive a complete
`wchar_t` terminator instead of one extra byte.  Both conversion paths also
return `NULL` when their allocation fails before writing through the result.

The existing position bound remains in force because the legacy source reader
uses an `int` position.  `tests/automated/check-text-source-range-allocation.py`
protects the checked size calculation and both allocation guards.
