# XmString rendition-tag bounds

Segment parsing checks the rendition-tag count before allocating copied or
borrowed tag arrays, including Xt's `Cardinal` byte-size range. If a malformed
context reports an unrepresentable count, parsing stops cleanly with no tag
array returned; ordinary rendition ordering and ownership are unchanged.

The source-level regression is
`tests/automated/check-xmstring-rendition-bounds.py`.
