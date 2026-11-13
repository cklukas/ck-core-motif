# XmString segment failure cleanup

`_XmStringGetSegment` can obtain owned component copies before discovering an
invalid legacy count or failing rendition-tag allocation.  Failed extraction
now releases the owned rendition tags and the copies made so far before
returning `FALSE`, and leaves all output pointers in a deterministic empty
state.  The text and the tag are borrowed from the string and its tag cache
until they are copied, so they are never released on a failure; see
[xmstring-stream-bounds.md](xmstring-stream-bounds.md).

The focused source-contract check is:

    python3 tests/automated/check-xmstring-segment-failure-cleanup.py
