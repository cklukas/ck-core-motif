# XmString rendition input validation

`XmStringPutRendition` now rejects a null source string or rendition tag
before measuring or constructing rendition components.  It returns `NULL`
without dereferencing invalid input.

The regression check is
`tests/automated/check-xmstring-rendition-input.py`.
