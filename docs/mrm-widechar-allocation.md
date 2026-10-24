# Mrm wide-character allocation handling

Wide-character literal fixup now checks the initial conversion buffer and the
resize result before publishing the pointer.  Failed conversion allocations
return `MrmFAILURE` and release owned storage instead of dereferencing or
losing an unchecked pointer.

The source-level regression is
`tests/automated/check-mrm-widechar-allocation.py`.
