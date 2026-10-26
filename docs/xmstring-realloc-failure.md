# XmString concat realloc failure

`XmStringConcatAndFree` now checks all reallocations used while merging
compatible segments.  The optimized segment and ordinary text paths preserve
the original allocation on failure, release the owned input strings, and
return `NULL` before later metadata or text writes can dereference an invalid
pointer.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-realloc-failure.py
```
