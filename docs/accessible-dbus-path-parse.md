---
nav_exclude: true
---
# AT-SPI object-path parsing

The optional D-Bus accessibility bridge accepts object paths below
`/org/a11y/atspi/accessible/` by parsing their numeric identifier. The parser
now requires a decimal digit at the start, checks `strtoul` for `ERANGE`, and
only then narrows the value to `XmAccessibleId`. This keeps valid paths
unchanged and prevents signed, empty, or overflowing identifiers from being
accepted differently on 32-bit and 64-bit hosts.
