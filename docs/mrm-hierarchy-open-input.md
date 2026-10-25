---
nav_exclude: true
---
# Mrm hierarchy-open input validation

Hierarchy opening now rejects a missing output pointer, a missing filename
list when files are requested, and a missing in-memory UID buffer before
opening displays or allocating hierarchy state. Valid file-backed and
in-memory opens retain their existing behavior.

The source-level regression is
`tests/automated/check-mrm-hierarchy-open-input.py`.
