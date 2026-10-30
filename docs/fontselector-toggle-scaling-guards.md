---
nav_exclude: true
---
# FontSelector scaling callback guards

The scaling callback now rejects missing callback data, incomplete font state,
and an unavailable family table. Resetting the size control is conditional on
the size widget existing, while valid callbacks retain the normal update and
font-refresh behavior.
