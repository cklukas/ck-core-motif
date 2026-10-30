# XmString segment-copy cleanup

`_XmStringGetSegment` asks the component iterator for owned copies before
normalizing a segment.  If a second allocation fails or a count is invalid,
the function now frees those intermediate tag and text copies before clearing
the outputs.  This prevents leaks during wide-character conversion and other
copy-data failure paths.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-segment-copy-cleanup.py
