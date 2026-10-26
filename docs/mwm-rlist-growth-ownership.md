# MWM rectangle-list growth ownership

`ExtendRList` now treats a successful replacement allocation as the new list
buffer even when the old list is empty. The old implementation only assigned
the replacement pointer inside the non-empty copy branch, so growing a zero-
capacity list leaked the new allocation and left `prect` unchanged.

The growth path also rejects an inconsistent nonzero `used` count with no old
buffer, validates the list counters before copying, and keeps the old list
untouched when validation fails. The focused source contract is covered by
`tests/automated/check-mwm-rlist-growth-ownership.py`.
