---
nav_exclude: true
---
# Command-palette item allocation

`XmCommandPaletteSetItems` now rejects a `NULL` item array for a nonzero
count and handles item-array allocation failure before populating entries.
The internal XmString copy helper also rejects a failed empty-string fallback
before calling the Motif constructor.
Filtering also handles failure to allocate its visible-index array before
writing matches, and skips an individual item if its copied display strings,
combined display buffer, or `XmString` cannot be allocated.  It also rejects
size-overflowing combined display lengths.  The palette remains usable and
can be populated again later; valid arrays and the public item-copying
contract are unchanged.

`tests/automated/check-command-palette-item-allocation.py` protects these
failure paths.
