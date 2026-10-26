# Optimized XmString rendition allocation

The optimized-source branch of `XmStringConcatAndFree` now checks its
single-tag rendition allocation before storing the tag.  Allocation failure
releases the owned input strings and returns `NULL`, matching the checked
multi-tag branch and preventing a null-pointer write.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-optimized-rendition-allocation.py
```
