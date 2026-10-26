---
nav_exclude: true
---
# Shared varargs widget-creation parent guard

`XmeVLCreateWidget` now returns `NULL` when its parent widget is null, before
application-context lookup or varargs conversion. This protects all public
varargs constructors that use the shared helper while preserving valid widget
creation.

The focused regression is
`tests/automated/check-vl-create-parent-input.py`.
