---
nav_exclude: true
---
# DropDown layout state

`XmDropDown` now starts a no-resize layout pass from its current core geometry
and initializes optional label metrics to zero. This makes unrealized and
partially constructed control layout deterministic, rather than depending on
uninitialized local dimensions. Normal requested-size negotiation and visible
label layout are unchanged.

The focused regression is
`tests/automated/check-dropdown-negotiation-contract.py`.
