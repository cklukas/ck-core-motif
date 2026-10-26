---
nav_exclude: true
---
# Command-palette item transactions

`XmCommandPaletteSetItems` now installs an item batch transactionally. If an
ID, label, or shortcut copy fails, all partial records are released and the
palette remains empty rather than exposing partially initialized callbacks or
filter entries.

The focused source check is
`tests/automated/check-command-palette-item-allocation.py`.
