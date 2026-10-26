# MWM all-client query guards

The WM-query conversion path now validates its screen index and all output
pointers before traversing the client list. Detached entries without client
data are skipped, and the accumulated XID count is explicitly widened to the
selection API's `unsigned long` length type.

The source contract is covered by
`tests/automated/check-mwm-query-client-guards.py`.
