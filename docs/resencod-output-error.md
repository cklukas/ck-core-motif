# Compound-text output error propagation

Compound-text decoding builds an `XmString` incrementally.  If one of those
allocations fails, the decoder now records the output-construction failure and
returns conversion failure instead of silently continuing with a partial or
empty result.  Existing fallback charset handling remains unchanged; only a
failed final `XmString` construction changes the result.

The focused source-contract check is:

    python3 tests/automated/check-resencod-output-error.py
