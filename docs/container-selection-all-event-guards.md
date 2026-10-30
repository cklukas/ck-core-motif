---
nav_exclude: true
---
# Container select-all event guards

The Container select-all and deselect-all actions now return safely when
invoked without an event.  Their primary-selection timestamps and callbacks
require that event; valid keyboard and pointer actions retain their existing
behavior.

The source contract is checked by
`tests/automated/check-container-selection-all-event-guards.py`.
