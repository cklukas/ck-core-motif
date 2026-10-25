---
nav_exclude: true
---
# I18List target-list conversion allocation

I18List now checks `XmeStandardTargets` before writing the target list.  Its
export/clipboard target list also reserves space for the optional pixmap atom;
previously that atom could write past a five-entry allocation.

The focused regression is
`tests/automated/check-i18list-target-list-allocation.py`.
