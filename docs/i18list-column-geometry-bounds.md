---
nav_exclude: true
---
# I18List column geometry bounds

Column widths are stored in Motif's legacy `short` table.  `XmI18List` now
resets each column's measured maximum, clamps a measured width before storing
it, and accumulates the horizontal scrollbar range in `int64_t` with an
`INT_MAX` ceiling.  This prevents one column's measurement from leaking into
the next column and prevents a large column set from wrapping the scrollbar
maximum.

The focused source regression is
`tests/automated/check-i18list-column-geometry-bounds.py`.
