# FileSB directory-list allocation failure

File Selection Box directory navigation converts matching directory names into
an `XmString` array before updating the directory list.  The array allocation
is now checked before any entries are written.  If it fails, all temporary
directory-name strings are released and the callback returns safely without
passing an invalid array to the list widget.

The regression check is
`tests/automated/check-filesb-directory-list-allocation.py`.
