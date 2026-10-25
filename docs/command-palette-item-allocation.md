---
nav_exclude: true
---
# Command-palette item allocation

`XmCommandPaletteSetItems` now rejects a `NULL` item array for a nonzero
count and handles item-array allocation failure before populating entries.
The palette remains empty and can be populated again later; valid arrays and
the public item-copying contract are unchanged.

`tests/automated/check-command-palette-item-allocation.py` protects these
failure paths.
