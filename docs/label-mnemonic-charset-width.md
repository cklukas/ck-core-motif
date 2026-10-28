# Label mnemonic charset width

The Label `XmNmnemonicCharSet` get-values hook only needs to distinguish an
empty string from a non-empty one.  It now checks the first character directly
instead of narrowing `strlen()` from `size_t` to `int`, so long valid charset
names remain exportable.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-label-mnemonic-charset-width.py
```
