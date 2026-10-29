---
nav_exclude: true
---
# Toast child allocation

Toast creates message and action children during initialization.  Geometry,
callback registration, and resource updates now guard missing children so an
allocation failure cannot crash a later redraw or `SetValues` operation.

The contract is protected by
`tests/automated/check-toast-child-failure-guards.py`.
