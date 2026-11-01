---
nav_exclude: true
---

Normalize malformed transfer callback results
==============================================

The internal converter path also releases a converter-owned payload when its
format, type, length, or pointer combination is malformed, or when conversion
does not complete.  Rejected output is cleared before callers observe the
failure, preventing a leak while preserving the existing failure contract.

`SelectionCallbackWrapper` now validates selection-owner results before
publishing them through `XmSelectionCallbackStruct`.  A missing selection atom
and invalid 8/16/32-bit
formats and non-empty replies without both a type atom and payload pointer are
released, converted to an empty result, and reported with `XmCR_PROTOCOLS`
instead of `XmCR_OK`.  Valid results retain their original type, format,
length, and callback reason. A zero-length reply may carry either a null pointer
or an Xt-allocated empty value. Both are valid selection results, including the
empty acknowledgement used by WSM SET_STATE. The receiving callback retains
ownership of any delivered allocation and releases it normally.

The focused regression is:

```text
python3 tests/automated/check-transfer-callback-payload.py
```

The native MWM/Wsm acceptance runner exercises empty SET_STATE acknowledgements
through the real selection-transfer callback path.
