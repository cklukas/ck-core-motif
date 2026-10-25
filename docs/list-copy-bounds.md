# List copy allocation bounds

List copy-on-write helpers now validate count products and allocation results
before copying items or selected positions. A failed copy leaves the original
array owned by the widget instead of dereferencing a null temporary buffer.

The source-level regression is
`tests/automated/check-list-copy-bounds.py`.
