---
nav_exclude: true
---
# Notebook child lookup constraint guard

The internal Notebook child lookup now skips null composite children and
children without constraint storage before reading page/type metadata. This
keeps malformed child state from crashing tab and page discovery while leaving
valid lookup behavior unchanged.

The source contract is checked by
`tests/automated/check-notebook-child-widget-constraint-guard.py`.
