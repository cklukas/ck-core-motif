# XmStringNConcat copy-limit bounds

`XmStringNConcat()` derives the `XmStringNCopy()` limit by adding the first
string length and the requested byte count.  The addition is now checked in
the signed `int` API domain; negative or unrepresentable limits return `NULL`
instead of wrapping into a smaller copy.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-xmstring-nconcat-bounds.py
```
