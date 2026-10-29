# ComboBox selected-position import input

The synthetic selected-position import procedure receives an `XtArgVal`
pointer from resource conversion.  A malformed or private caller can provide
no widget or no value pointer; dereferencing either before the child-creation
check would crash the process.  The import now returns the existing
`XmSYNTHETIC_NONE` result for null inputs and keeps valid resource conversion
unchanged.
Zero-based imports also reject `XmINVALID_POSITION` and other negative values
before applying the compatibility `+1` conversion, preventing `-1` from
becoming List position zero (which means the last item).

The regression contract is checked by
`tests/automated/check-combobox-selected-import-input.py`.
