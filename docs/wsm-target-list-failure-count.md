---
nav_exclude: true
---

# WSM target-list failure count

`WSMGetTargetList` now clears its caller-provided output count before checking
the widget and display state.  A null widget, unavailable display record, or
allocation failure therefore returns a null list with a count of zero.  This
prevents callers from pairing a failed result with a stale nonzero count while
leaving successful target-list contents and ordering unchanged.  The allocation
also verifies its byte count fits Xt's `Cardinal` interface before narrowing.

The focused regression is
`tests/automated/check-wsm-target-list-failure-count.py`.
