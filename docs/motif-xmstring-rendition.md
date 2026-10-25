# XmString rendition input validation

`XmStringPutRendition` now rejects a null XmString or rendition tag before
calling `strlen` or creating rendition components.  It returns `NULL` instead
of dereferencing invalid input.

The regression check is
`tests/automated/check-xmstring-rendition-input.py`.
