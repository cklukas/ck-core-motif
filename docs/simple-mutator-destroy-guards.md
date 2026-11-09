---
nav_exclude: true
---
# Simple mutator teardown guards

`XmPromptSetVisible` and `XmDropHighlightSetState` now reject calls after Xt
has begun widget destruction. They no longer mutate redraw state or request an
X11 expose against teardown state. The behavior of live widgets, including
DropHighlight's `Boolean` result for valid and invalid states, is unchanged.

The focused check is
`tests/automated/check-simple-mutator-destroy-guards.py`.
