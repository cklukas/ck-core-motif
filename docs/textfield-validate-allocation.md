# TextField validation allocation safety

`ValidateString` filters a new value before storing it in an `XmTextField`.
Both the narrow and wide-character branches now retain the `XtMalloc` result,
check it, and only then copy the validated bytes. Allocation failure leaves an
empty, internally consistent value instead of passing a null pointer to
`memcpy`.

The regression check is:

```sh
python3 tests/automated/check-textfield-validate-allocation.py
```
