---
nav_exclude: true
---

# Badge teardown input

Badge destruction now validates the widget class before freeing its drawing
context, font, and label state. Invalid teardown dispatch returns safely;
normal widget destruction is unchanged.

The focused source contract is checked by
`tests/automated/check-badge-destroy-input.py`.
