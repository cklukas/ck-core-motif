---
nav_exclude: true
---
# SimpleMenu creation-failure handling

SimpleMenu assembly now stops safely when its RowColumn widget is unavailable
and only registers callbacks for children that Xt actually created. This
prevents an allocation or widget-creation failure from becoming a secondary
NULL dereference while preserving successful menu construction.

The focused regression is
`tests/automated/check-simple-menu-create-failures.py`.
