---
nav_exclude: true
---
# Hierarchy child-count loops

Hierarchy child-list traversal, insertion, removal, state propagation, and
layout-table construction now use `Cardinal` indices for their `Cardinal`
counts. Reverse insertion shifts count down explicitly instead of relying on a
negative signed sentinel. This preserves behavior while keeping large valid
hierarchies within the width of their Xt count fields.

The focused source regression is
`tests/automated/check-hierarchy-count-bounds.py`.
