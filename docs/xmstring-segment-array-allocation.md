# XmString segment-array allocation

When concatenation combines implicit lines, `XmStringConcatAndFree` grows or
creates a segment-pointer array.  The combined segment count is now checked
before multiplication, and both realloc/malloc results are checked before the
array is written.  Failure releases the owned inputs and returns `NULL`
without leaving a partially installed segment array.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-segment-array-allocation.py
```
