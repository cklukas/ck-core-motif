# XmString context copied-component failures

Context enumeration now checks copied rendition and tag strings before taking
their length. If `XtNewString` cannot allocate, enumeration reports end of
context instead of dereferencing `NULL`; the context error flag is updated when
the caller is advancing it.

The source contract is covered by
`tests/automated/check-xmstring-context-copy-failure.py`.
