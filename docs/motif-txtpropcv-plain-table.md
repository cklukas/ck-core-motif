---
nav_exclude: true
---
# Non-empty plain text-property table allocation

After counting NUL-separated locale, ISO-8859-1, or UTF-8 items,
`XmCvtTextPropertyToXmStringTable` now validates the table-size multiplication
and passes the checked byte count explicitly as an Xt `Cardinal` before
generating each `XmString`. Invalid sizes or allocation failure return
`BadAlloc` with balanced locking.

The regression check is
`tests/automated/check-txtpropcv-plain-table.py`.
