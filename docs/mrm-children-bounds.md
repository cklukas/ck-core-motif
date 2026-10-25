---
nav_exclude: true
---
# Mrm child-list bounds

Mrm child-list initialization now rejects zero counts before subtracting one
for the inline descriptor entry, and checks the resulting descriptor size
against the format's 16-bit `MrmSize` field. Positive valid child counts retain
their existing record layout and initialization behavior.

The source-level regression is `tests/automated/check-mrm-children-bounds.py`.
