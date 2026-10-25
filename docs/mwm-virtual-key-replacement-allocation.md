---
nav_exclude: true
---

# MWM virtual-key replacement allocation

The optional HP/DIN virtual-key compatibility path validates the X keymap
status and keycode/keysym count arithmetic before scanning. It also grows a
replacement table with a record-count check against Xt's `Cardinal` width and
handles `XtRealloc` failure by releasing the partial table and X keymap before
returning without applying remappings.

The regression is covered by
`tests/automated/check-mwm-virtual-key-replacement-allocation.py`.
