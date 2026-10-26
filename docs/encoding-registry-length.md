---
nav_exclude: true
---
# Encoding-registry target length

The encoding-registry helper reports a validated non-negative `int` length,
while selection callbacks store lengths as `unsigned long`. The standard
converter now widens that value explicitly, preserving the checked contract
without an implicit sign conversion.

The focused regression is
`tests/automated/check-encoding-registry-length.py`.
