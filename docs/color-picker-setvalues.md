---
nav_exclude: true
---
# ColorPicker live resource updates

The native ColorPicker applies value, render-table and appearance changes together.
A value update synchronizes the hex editor, visual fields, model channels and
alpha swatch. Owned child fonts follow a replacement render table while explicit
child overrides remain intact. A failed render-table copy retains the valid table.

Mode switching preserves pending text and the current color. Geometry is measured
from native controls, including the actual hint font; explicit axes remain owned
by the caller. Cached field pixels are invalidated for changes to the color-space
parameter, allocation, sensitivity, background or colormap.

Native checks: `run-colorpicker-native.py` and `run-colorpicker-modes.py`.
