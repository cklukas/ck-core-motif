---
nav_exclude: true
---

# Badge SetValues input

Badge `SetValues` now validates the old and new widget records before comparing
font, label, and severity state. Malformed lifecycle calls return `False`
without touching private state; valid resource updates are unchanged.

The focused source contract is checked by
`tests/automated/check-badge-set-values-input.py`.
