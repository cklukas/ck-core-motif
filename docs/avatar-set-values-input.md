---
nav_exclude: true
---

# Avatar SetValues input

Avatar `SetValues` now validates its old and new widget records before
comparing font and initials state. Malformed lifecycle calls return `False`
without touching private state; valid resource updates are unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-set-values-input.py`.
