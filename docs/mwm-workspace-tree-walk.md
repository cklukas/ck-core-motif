---
nav_exclude: true
---

# MWM workspace transient-tree traversal

The standalone MWM workspace protocol may inspect every transient client when
it answers `WSM_GET_STATE`, restores visibility after a workspace-manager
change, or resolves a registered client window.  A valid transient hierarchy
can be much deeper than a process call stack, so recursive traversal could
terminate MWM while answering an otherwise ordinary workspace request.

`WmWorkspace.c` now uses one explicit, heap-backed depth-first frame stack.
It retains the prior parent-before-child and sibling order, stops safely if
its stack cannot grow, and bounds `WSM_GET_STATE` to the protocol's CARD16
entry limit.  If a reply snapshot cannot be completed, already-owned
per-window data is released and an empty, valid reply is returned rather than
publishing a partial ownership graph.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-workspace-tree-walk.py
```
