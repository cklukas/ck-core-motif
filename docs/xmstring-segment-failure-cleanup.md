# XmString segment failure cleanup

`_XmStringGetSegment` can obtain owned component copies before discovering an
invalid legacy count or failing rendition-tag allocation.  Failed extraction
now releases every owned text, tag, and rendition output before returning
`FALSE`, preventing leaks and leaving all output pointers in a deterministic
empty state.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-segment-failure-cleanup.py
