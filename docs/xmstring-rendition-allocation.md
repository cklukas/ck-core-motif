# XmString rendition-tag allocation

`XmStringConcatAndFree` now validates rendition-end tag counts before
allocating the copied tag array and checks the allocation result before
copying tags.  On failure it releases the owned input strings and returns
`NULL`, matching the surrounding concat allocation paths instead of
dereferencing a null array.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-rendition-allocation.py
```
