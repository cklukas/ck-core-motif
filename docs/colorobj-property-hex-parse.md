---
nav_exclude: true
---

# Color-object property parsing

Color-object pixel properties are supplied as underscore-delimited
hexadecimal fields from an X-server selection. The parser now uses checked
`strtoul` conversion, rejects signs, overflow, and missing delimiters, and
validates the color-use value before storing it.

The focused regression check is
`tests/automated/check-colorobj-property-hex-parse.py`.
