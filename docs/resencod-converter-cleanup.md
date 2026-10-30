# Compound-text converter failure cleanup

Compound-text conversion can build an output buffer incrementally.  If a
later segment cannot be converted, or an append allocation fails, the
converter now releases the partial output buffer, the active encoding copy,
and the string traversal context before returning failure.  This keeps the
public `XmCvtXmStringToCT` and UTF-8 conversion paths leak-free under malformed
input and allocation pressure without changing their failure result.

The focused source-contract check is:

    python3 tests/automated/check-resencod-converter-cleanup.py
