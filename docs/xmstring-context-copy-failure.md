# XmString context-copy failure state

`_XmStringContextCopy` duplicates active rendition state for look-ahead and
unparse operations.  If the source state is malformed or the duplicate cannot
be allocated, the target context now records an error instead of looking like
a valid context with silently missing renditions.  Subsequent component
iteration stops through the existing context-error path.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-context-copy-failure.py
