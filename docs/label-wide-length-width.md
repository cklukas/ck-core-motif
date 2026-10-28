# Label wide-string length width

The Label wide-character resource conversion now scans the input with
`size_t`.  This keeps length tracking and the subsequent multibyte allocation
in the same wide range, avoiding an `int` counter wrap for unusually large
wide strings.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-label-wide-length-width.py
```
