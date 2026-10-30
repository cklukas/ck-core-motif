---
nav_exclude: true
---
# Container managed-child constraint guards

`XmContainer`'s `ChangeManaged` method now skips a managed child when its
Container constraint record is unavailable.  The method can be reached while
children are being added or removed; a missing record must not be passed to
`NodeIsActive` or outline-state checks.  Valid children keep the existing
visibility and layout behavior.

The source contract is checked by
`tests/automated/check-container-managed-constraint-guards.py`.
