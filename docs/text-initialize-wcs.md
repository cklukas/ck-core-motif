# Text initialization wide-string conversion

When an existing text source receives `XmNwcValue`, initialization now bounds
the conversion-buffer size and checks allocation before calling `wcstombs` or
writing the terminator. Invalid conversion or unavailable storage leaves the
existing source value unchanged while initialization continues safely.

The focused source contract is checked by
`tests/automated/check-text-initialize-wcs.py`.
