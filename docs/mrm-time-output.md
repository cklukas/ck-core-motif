---
nav_exclude: true
---
# Mrm timestamp output validation

The internal timestamp helper now returns immediately when given a null output
buffer, avoiding a crash in malformed header-construction calls. Valid calls
retain the existing platform timestamp formatting and destination contract.

The source-level regression is `tests/automated/check-mrm-time-output.py`.
