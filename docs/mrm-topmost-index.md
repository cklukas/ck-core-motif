---
nav_exclude: true
---
# Mrm interface-module topmost indices

`UrmIFMSetTopmost` now rejects null or overlong indices before touching the
fixed-width descriptor field.  Valid calls replace the previous index instead
of appending to it, preventing repeated updates from overflowing the field and
making the setter behave as its name indicates.

The source-level regression is
`tests/automated/check-mrm-topmost-index.py`.
