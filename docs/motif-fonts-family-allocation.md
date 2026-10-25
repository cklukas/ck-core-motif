# Font selector family-list allocation

Font selector family refresh now checks the temporary `XmString` array size
and allocation before filling it. If the array cannot be created, the refresh
returns with the existing family list intact instead of dereferencing a null
pointer.

The source-level regression check is
`tests/automated/check-fonts-family-allocation.py`.
