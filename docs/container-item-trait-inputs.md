---
nav_exclude: true
---
# Container item-trait input validation

Visual-emphasis and view-type trait helpers now return safe defaults for a
missing child widget, and visual-emphasis updates return when the child has no
Container parent. Valid trait dispatch is unchanged.

The source contract is checked by
`tests/automated/check-container-item-trait-inputs.py`.
