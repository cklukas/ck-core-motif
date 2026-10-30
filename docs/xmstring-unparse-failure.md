# XmStringUnparse failure propagation

`XmStringUnparse` now reports allocation, overflow, and character-conversion
failures from its text helper.  The helper marks the output length invalid;
the public API frees any partial result and returns `NULL` instead of silently
returning truncated or incomplete text.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-unparse-failure.py
