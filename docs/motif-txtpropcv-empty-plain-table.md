---
nav_exclude: true
---
# Empty plain text-property table allocation

For empty locale, ISO-8859-1, or UTF-8 text properties,
`XmCvtTextPropertyToXmStringTable` returns one generated empty `XmString`.  The
result table allocation is now checked and reports `BadAlloc` with balanced
application locking if it fails.

The regression check is
`tests/automated/check-txtpropcv-empty-plain-table.py`.
