---
nav_exclude: true
---
# Container cursor-navigation input validation

Cursor navigation now returns safely for missing widgets or direction strings,
and `_LAST` navigation checks that the relevant child nodes and widget pointers
exist before traversal. Valid spatial and outline navigation is unchanged.

The source contract is checked by
`tests/automated/check-container-cursor-navigation-inputs.py`.
