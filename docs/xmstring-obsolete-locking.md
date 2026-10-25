# Legacy XmString constructor locking

`XmStringLtoRCreate` now delegates synchronization to the direction,
concatenation and separator constructors it calls. The compatibility API no
longer holds the process lock across those nested operations, avoiding
recursive locking while preserving its input validation and returned
`XmString` ownership contract.

The source contract is covered by
`tests/automated/check-xmstring-obsolete-locking.py`.
