---
nav_exclude: true
---
# MWM restart-command failure recovery

During session checkpointing, MWM builds an owned replacement for the
session-manager restart command so that its current database path is present.
That construction can fail cleanly when memory is unavailable or the existing
argument vector cannot be represented safely.

MWM now leaves the session manager's existing restart command unchanged in
that case.  It still refreshes the discard command for the current database.
Normal successful checkpoints retain the same replacement command and argument
ordering as before.
