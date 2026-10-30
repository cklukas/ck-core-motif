---
nav_exclude: true
---
# Container fit-test constraint guards

`TestFitItem` now returns `False` when the candidate child has no Container
constraint record.  Grid and cell placement consult `cell_idx` from that
record; rejecting malformed input keeps hit testing and placement checks from
dereferencing unavailable private state.  Valid candidates retain their
existing fit behavior.

The source contract is checked by
`tests/automated/check-container-fit-constraint-guards.py`.
