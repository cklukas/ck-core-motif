# List and ComboBox callback length bounds

`XmListCallbackStruct.item_length` is part of the public Motif ABI and is an
`int`, while `XmStringLength()` returns an unsigned length.  List and ComboBox
now clamp oversized compound-string lengths to `INT_MAX` before populating the
callback record or the List's internal cache, preventing signed wraparound
without changing the ABI.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-list-callback-length-bounds.py
```
