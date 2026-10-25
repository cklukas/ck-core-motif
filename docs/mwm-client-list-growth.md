---
nav_exclude: true
---
# MWM client-list growth

Session-management discovery grows a list of client pointers as clients are
found.  Growth now validates the count and byte multiplication and leaves the
existing list untouched if `XtRealloc` fails.  Valid additions retain their
order and count behavior.

`tests/automated/check-mwm-client-list-growth.py` protects this contract.
