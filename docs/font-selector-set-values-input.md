---
nav_exclude: true
---

# FontSelector SetValues input

`SetValues` now validates all widget records and the argument list/count before
filtering child resources or comparing private state. Malformed calls return
`False` without changing widget state; normal Xt resource updates retain their
existing behavior.

The focused source contract is checked by
`tests/automated/check-font-selector-set-values-input.py`.
