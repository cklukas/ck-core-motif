---
nav_exclude: true
---
# Mrm name-registration counts

The public Mrm name-registration APIs now reject negative or unrepresentable
entry counts before allocating parallel name and value vectors. A zero-entry
registration is treated as a successful no-op, and the internal global and
hierarchy registration paths reject negative counts as well. Valid registrations
retain their existing behavior.

The source-level regression is `tests/automated/check-mrm-register-counts.py`.
