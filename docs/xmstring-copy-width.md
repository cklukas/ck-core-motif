---
nav_exclude: true
---
# Copied wide XmString segment width

The internal segment reader can copy component data for callers that request
owned text.  The count that it holds for a text component is a number of bytes
for every text type, wide-character text included (the component iterator
reports the byte count of the segment).  An earlier change multiplied the count
by `sizeof(wchar_t)` for wide text, which read four times the data of the
segment.  The copy takes the byte count for all types, pads it to a whole number
of wide characters and ends it with a null wide character; see
[xmstring-stream-bounds.md](xmstring-stream-bounds.md).  The source regression in
`tests/automated/check-xmstring-copy-width.py` protects the copy, and
`tests/automated/check-xmstring-stream-bounds.c` runs it on a wide text stream.
