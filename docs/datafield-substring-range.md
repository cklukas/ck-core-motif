# DataField substring range validation

`XmDataFieldGetSubstringWcs` now rejects negative starts and lengths and checks
the start against the current string before subtracting or forming a pointer.
Valid requests that extend past the end retain the existing truncated-copy
behavior; malformed ranges return `XmCOPY_FAILED`.

The regression check is:

```sh
python3 tests/automated/check-datafield-substring-range.py
```
