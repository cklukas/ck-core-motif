# DataField selection conversion bounds

DataField selection imports can contain several text fragments.  The
conversion path now accumulates their byte lengths in `size_t`, rejects a
sum that cannot be represented by the widget's `int` length contract, and
checks the wide-character allocation before multiplying by `sizeof(wchar_t)`.
An empty or unconvertible text-property list is represented by an allocated
empty string, so the replacement path never receives a null buffer for a
zero-length conversion.  Failed conversions release the selection payload
and report failure to the selection owner.

The `check-selection-timestamp` regression exercises the DataField
`XA_STRING` conversion path after rebuilding the library.
