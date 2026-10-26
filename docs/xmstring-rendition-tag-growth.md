---
nav_exclude: true
---
# XmString rendition-tag growth

Rendition begin/end tag arrays grow from an internal `int` count. The growth
paths now reject a count that cannot be incremented or whose resulting byte
size cannot be represented by either `size_t` or Xt's `Cardinal`, and perform
the multiplication only after those checks. Existing tag ordering and public
XmString behavior are unchanged for representable inputs.

The focused regression is
`tests/automated/check-xmstring-rendition-tag-growth.py`.
