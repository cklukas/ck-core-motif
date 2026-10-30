---
nav_exclude: true
---
# FontSelector font-string construction guards

`BuildFontString` now rejects incomplete selector/font/buffer inputs and
converts missing family, weight, or encoding quarks to the existing XLFD
wildcard. Valid font selections keep the same output format, while malformed
state no longer reaches a null `%s` argument or an invalid buffer write.
