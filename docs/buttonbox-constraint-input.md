---
nav_exclude: true
---

# ButtonBox constraint hook input

ButtonBox child constraint initialization and SetValues now reject a missing
`set` widget before accessing per-child geometry. Valid child layout and
preferred-size invalidation remain unchanged.

The focused source contract is checked by
`tests/automated/check-buttonbox-constraint-input.py`.
