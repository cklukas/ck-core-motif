---
nav_exclude: true
---
# Unit-string byte classification

Motif unit parsing now converts each input byte to `unsigned char` before
calling `isascii` and `isspace`. This keeps whitespace handling defined for
high-bit bytes in localized resource strings without changing valid numeric
or unit syntax.
