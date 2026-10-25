---
nav_exclude: true
---
# MWM client-list growth

Session-management discovery grows a list of client pointers as clients are
found.  Growth now validates the count, byte multiplication, and Xt's
`Cardinal` width, leaving the existing list untouched if `XtRealloc` fails.
Valid additions retain their order and count behavior.

`tests/automated/check-mwm-client-list-growth.py` protects this contract.
