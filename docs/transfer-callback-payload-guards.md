Normalize malformed transfer callback results
==============================================

`SelectionCallbackWrapper` now validates selection-owner results before
publishing them through `XmSelectionCallbackStruct`.  A missing selection atom
and invalid 8/16/32-bit
formats and non-empty replies without both a type atom and payload pointer are
released, converted to an empty result, and reported with `XmCR_PROTOCOLS`
instead of `XmCR_OK`.  Valid results retain their original type, format,
length, and callback reason; zero-length replies must not carry an allocated
payload either.

The focused regression is:

```text
python3 tests/automated/check-transfer-callback-payload.py
```
