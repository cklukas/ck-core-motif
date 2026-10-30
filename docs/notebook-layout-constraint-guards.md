---
nav_exclude: true
---
# Notebook layout constraint guards

Page, major-tab, and minor-tab layout scans now skip null composite children
and children without constraint storage before reading Notebook metadata. This
prevents malformed child state from crashing relayout while preserving normal
geometry and tab behavior.

The source contract is checked by
`tests/automated/check-notebook-layout-constraint-guards.py`.
