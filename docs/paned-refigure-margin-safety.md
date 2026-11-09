---
nav_exclude: true
---

# Paned refigure and margin safety

XmPaned now keeps narrow geometry negotiation in the representable Dimension
range in both directions. Adding margins saturates, while removing them floors
at zero instead of wrapping into a very large size. This applies to parent
negotiation, returned geometry, preferred geometry, reset, and set-values
refigure paths.

The no-unused-space fallback also handles an all-pane refigure (NO_INDEX)
without indexing before the pane array. It uses the last pane for a global
refigure and the first pane when a specified first pane has no predecessor.

The focused regression check is
tests/automated/check-paned-refigure-margin-safety.py.
