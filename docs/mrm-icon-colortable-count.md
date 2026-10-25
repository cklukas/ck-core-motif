# Mrm icon color-table count validation

A color table always reserves entries zero and one for background and
foreground pixels. `Urm__RealizeColorTable` now rejects a table whose count is
below `URMColorTableUserMin` before writing those reserved entries. This
prevents malformed UID data with a zero or one entry from causing an
out-of-bounds write while preserving all valid color tables.
