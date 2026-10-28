# SelectionBox callback length bounds

`XmSelectionBoxCallbackStruct.length` is a signed `int`, while the generated
compound string reports an unsigned length.  SelectionBox now clamps the
length before publishing the callback record, preventing signed wraparound for
very large Unicode values without changing the public ABI.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-selection-callback-length-bounds.py
```
