---
nav_exclude: true
---
# CommandPalette replacement input

`XmCommandPaletteSetItems` validates non-empty replacement input before freeing
the current item model. A NULL item array or an oversized count is rejected
without erasing the visible palette. Passing a zero count remains the explicit
operation for clearing/rebuilding the palette.

The focused source-contract check is
`tests/automated/check-command-palette-input-preservation.py`.
