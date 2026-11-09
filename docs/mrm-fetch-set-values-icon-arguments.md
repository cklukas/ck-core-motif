---
nav_exclude: true
---

# Mrm FetchSetValues icon arguments

`MrmFetchSetValues` now retains deferred icon arguments in checked dynamic
storage sized for the supplied argument list.  The historic fixed ten-entry
temporary array could overflow when a UID-driven `XtSetValues` operation
resolved more than ten icon-image literals.  Larger valid lists now retain all
their icon arguments; invalid or unrepresentable allocation sizes fail with
the established `MrmFAILURE` result.

The focused regression is
`tests/automated/check-mrm-fetch-set-values-icon-arguments.py`.
