# XmString concatenation copy failures

`XmStringConcat` copies both operands before handing ownership to
`XmStringConcatAndFree`. If copying the second non-NULL operand fails, the
function now frees the first copy and returns `NULL` instead of treating the
missing operand as an empty string and returning a partial result.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-concat-copy-failure.py
```
