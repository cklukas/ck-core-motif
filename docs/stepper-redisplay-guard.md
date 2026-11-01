---
nav_exclude: true
---
# Stepper rendering ownership

Stepper now uses native Label and PushButton children. It no longer creates a
private drawing GC, queries X font IDs, or maintains a separate text baseline.
Each native child owns its rendering resources and ordinary redisplay behavior.

The earlier custom-redisplay source guard has been superseded by
`tests/automated/run-stepper-native.py`, which exercises actual installed-library
geometry and interaction. See [Stepper](stepper.md) for the current contract.
