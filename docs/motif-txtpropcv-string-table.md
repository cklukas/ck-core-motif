# Compound-text string-table allocation

The compound-text branch of `XmCvtTextPropertyToXmStringTable` now checks its
result table allocation before handling empty or non-empty properties. A
failure returns `BadAlloc` with the application lock balanced instead of
writing through a null table.

The regression check is
`tests/automated/check-txtpropcv-string-table-allocation.py`.
