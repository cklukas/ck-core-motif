---
nav_exclude: true
---
# Render-table numeric token parsing

The render-table lexer now handles two malformed numeric cases safely.  An
unrecognized character is consumed as a one-character token instead of leaving
the lexer at the same position, and a `strtod` result is cast to `int` only
after checking the full integer range.  Out-of-range values remain floating
tokens without invoking an undefined conversion.
