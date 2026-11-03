---
nav_exclude: true
---
# UIL raw database table allocation

Several WMD records contain raw byte, `unsigned short`, or `int` tables.
Their loader now routes each allocation through a shared helper before passing
the result to `fread`.  An empty serialized table gets a one-byte object, so
the input API always receives a valid destination; an allocation failure is
diagnosed and terminates database loading.  In particular, the loader does not
mistake the unread record payload for a subsequent table header.

This does not alter the serialized database format or valid table contents.
`tests/automated/check-uil-db-raw-table-allocation.py` covers the loading
contract for every raw-table record type.
