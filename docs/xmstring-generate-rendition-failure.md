# XmStringGenerate rendition failure

When `XmStringGenerate` is asked to apply a rendition, it may convert compact
segments and grow rendition-tag arrays.  Allocation or size-check failures in
those steps now release the generated string and return `NULL`.  The function
no longer reports an unwrapped string as successful output when the requested
rendition could not be applied.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-generate-rendition-failure.py
