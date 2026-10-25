# Bounded compound-text segment copies

`XmCvtTextPropertyToXmStringTable` copies each bounded compound-text segment
into a temporary NUL-terminated buffer.  A failed segment allocation now frees
already converted `XmString` entries, releases the table, balances the
application lock, and returns `BadAlloc`.

The regression check is
`tests/automated/check-txtpropcv-segment-copy.py`.
