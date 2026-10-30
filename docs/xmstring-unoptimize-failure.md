# XmString optimized-entry conversion failures

Concatenation can convert optimized entries to their general representation.
Those conversions allocate memory and may fail. The conversion helper now
retains the original entry when allocation fails, and every concatenation path
checks for a `NULL` result before dereferencing or storing it. The public
operation releases its owned operands and returns `NULL` instead of crashing or
publishing a malformed string.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-unoptimize-failure.py
```
