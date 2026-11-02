---
nav_exclude: true
---

# Mrm FetchSetValues failure cleanup

`MrmFetchSetValues` allocates an internal pointer list before determining the
largest literal index.  If that index is too large for an Mrm resource record,
the routine now releases both the local argument list and the pointer list
before returning `MrmFAILURE`.

The focused regression is
`tests/automated/check-mrm-fetch-set-values-failure-cleanup.py`.
