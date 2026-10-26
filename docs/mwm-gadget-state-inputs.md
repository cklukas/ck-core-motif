---
nav_exclude: true
---
# MWM gadget-state input validation

`PushGadgetIn` and `PopGadgetOut` return without changing state when passed a
null `ClientData` pointer.  This guard runs before decoration flags, display
lists, or global gadget state are touched; valid gadget interactions are
unchanged.

The source-contract regression is
`tests/automated/check-mwm-gadget-state-inputs.py`.
