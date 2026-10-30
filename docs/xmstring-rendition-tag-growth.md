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

Rendition accessors also reject negative indices before touching the internal
tag arrays. This keeps malformed internal requests from indexing before an
array (or from starting a removal loop at `-1`) while leaving all valid
indices and public XmString behavior unchanged. The focused bounds regression
is `tests/automated/check-xmstring-rendition-tag-bounds.py`.

The ASN.1 parser applies the same pre-increment `INT_MAX` guard before growing
begin/end rendition arrays, so a malformed stream cannot wrap its count before
the allocation-width checks. `tests/automated/check-xmstring-parser-rendition-size.py`
covers that parser contract.
