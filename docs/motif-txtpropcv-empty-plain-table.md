# Empty plain text-property table allocation

For empty locale, ISO-8859-1, or UTF-8 text properties,
`XmCvtTextPropertyToXmStringTable` returns one generated empty `XmString`.  The
result table and empty-string generation are checked and report `BadAlloc` with
balanced application locking if either fails.

The regression check is
`tests/automated/check-txtpropcv-empty-plain-table.py`.
