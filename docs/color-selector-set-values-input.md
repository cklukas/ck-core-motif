---
nav_exclude: true
---

# ColorSelector SetValues input

`SetValues` now validates its current/request/set widget records and the
argument list/count before filtering child resources or reading private color
selector state. Malformed calls return `False`; normal resource updates are
unchanged.

The focused source contract is checked by
`tests/automated/check-color-selector-set-values-input.py`.
