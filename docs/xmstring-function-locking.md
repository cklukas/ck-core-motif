# XmString helper locking

`XmStringHasSubstring` and `XmeGetNextCharacter` now delegate synchronization
to the string helpers they call instead of holding the process lock across
those calls. This avoids recursive locking during substring scans and parse
callback component creation.

The source contract is covered by
`tests/automated/check-xmstring-function-locking.py`.
