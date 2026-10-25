---
nav_exclude: true
---

# MWM virtual-key replacement allocation

The optional HP/DIN virtual-key compatibility path grows a replacement table
as missing keysyms are discovered. Growth now checks the record count against
Xt's `Cardinal` width and handles `XtRealloc` failure by releasing the partial
table and X keymap before returning without applying remappings.

The regression is covered by
`tests/automated/check-mwm-virtual-key-replacement-allocation.py`.
