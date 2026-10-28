---
nav_exclude: true
---
# FileSelectionBox navigation action arguments

The legacy `_XmFileSelectionBoxUpOrDown` translation action accepts command
arguments `0` (up), `1` (down), `2` (home), and `3` (end). The argument is now
parsed as an exact bounded decimal value. Missing, malformed, overflowing, or
out-of-range values are ignored instead of being converted with unchecked
`atoi` arithmetic.

This keeps valid translation tables compatible while making malformed resource
or application-provided action arguments harmless.
