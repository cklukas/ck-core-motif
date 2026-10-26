---
nav_exclude: true
---
# Secondary-selection transfer inputs

Secondary-selection transfer now rejects a `None` target before creating a
request.  Its completion callback validates the widget and output pointers
before obtaining atoms, releasing any returned value when the callback state
is incomplete.  Invalid and cancellation paths therefore fail without
dereferencing stale selection state.

The focused regression is
`tests/automated/check-secondary-transfer-inputs.py`.
