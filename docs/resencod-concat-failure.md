# Compound-text concatenation failure handling

Compound-text conversion now uses a checked append API. ctextConcat updates
the output pointer and length only after XtRealloc succeeds; every encoder
caller returns a conversion failure immediately when the append cannot be
completed. This prevents length bookkeeping from advancing past the actual
buffer after an allocation failure.

The focused source-contract check is:

    python3 tests/automated/check-resencod-concat-failure.py
