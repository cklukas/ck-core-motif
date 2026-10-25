---
nav_exclude: true
---
# Unique-event state allocation

Motif keeps a small per-display stamp to identify duplicate menu events.  The
stamp allocation is now checked before publishing it into display state, and
event processing returns safely when the state cannot be created.  Existing
duplicate-event behavior is unchanged when allocation succeeds.

`tests/automated/check-unique-event-allocation.py` protects this contract.
