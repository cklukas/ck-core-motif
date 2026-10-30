---
nav_exclude: true
---

# Column label teardown guard

The Column label-destroy callback now validates its client field widget before
clearing the associated label pointer. Null or wrong-class callback payloads
are ignored safely; normal label destruction still clears the owning Column
field state.

The focused source-contract check is
`python3 tests/automated/check-column-label-destroy-guard.py`.
