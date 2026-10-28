---
nav_exclude: true
---
# I18List position conversion declaration

The I18List pixel-to-row/column conversion now declares its `pixel_y`
accumulator exactly once. A duplicate declaration in the same block prevented
the source from compiling with conforming C compilers.

The focused source regression is
`tests/automated/check-i18-list-position-declaration.py`.
