---
nav_exclude: true
---
# Menu command-match allocation

Menu command matching now treats its temporary `MatchList` allocation and
both copied strings as one operation.  A failure leaves the existing match
list unchanged and releases any partial allocation.

`tests/automated/check-menu-match-allocation.py` protects this path.
