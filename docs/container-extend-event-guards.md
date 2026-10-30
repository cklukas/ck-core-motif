---
nav_exclude: true
---
# Container keyboard-extend event guards

The Container keyboard extend action now returns safely when invoked without an
event.  Range selection and primary-selection notification both require the
event timestamp; malformed programmatic calls no longer dereference it.

The source contract is checked by
`tests/automated/check-container-extend-event-guards.py`.
