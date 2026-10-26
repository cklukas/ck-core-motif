---
nav_exclude: true
---
# ColorPicker live resource updates

`XmColorPicker::SetValues` now applies color, render-table, and value changes
in one pass before deciding whether to repaint. Foreground/background changes
refresh the label GC, and simultaneous render-table/value changes no longer
lose the value update through an early return. If a replacement render table
cannot be allocated, the previous valid table is retained.

The focused source contract is checked by
`tests/automated/check-color-picker-setvalues.py`.
