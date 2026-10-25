---
nav_exclude: true
---

# Render-table converter input validation

Render-table conversion now validates display, widget, source, destination, and
converter-argument inputs before creating renditions. Valid default, named,
and fallback font-list conversions remain unchanged.

The focused regression check is
`tests/automated/check-render-table-converter-inputs.py`.
