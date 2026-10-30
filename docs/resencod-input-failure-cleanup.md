# Compound-text input failure cleanup

Compound-text input conversion can build an `XmString` before discovering a
malformed escape/control sequence.  The conversion paths now free that
partial string on failure and only publish it to the caller after successful
parsing.  This avoids leaking partially converted strings and prevents a
failed conversion from leaving an ambiguous output object.

The focused source-contract check is:

    python3 tests/automated/check-resencod-input-failure-cleanup.py
