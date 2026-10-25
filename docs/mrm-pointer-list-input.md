---
nav_exclude: true
---
# Mrm pointer-list input validation

Mrm pointer-list initialization, resizing, append, and string-search helpers
now validate their list and string pointers before dereferencing them. Invalid
inputs return the existing `MrmFAILURE` or “not found” result; valid list
allocation, growth, ownership, and lookup behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-pointer-list-input.py`.
