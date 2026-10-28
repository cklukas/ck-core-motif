# ComboBox selected-position import input

The synthetic selected-position import procedure receives an `XtArgVal`
pointer from resource conversion.  A malformed or private caller can provide
no widget or no value pointer; dereferencing either before the child-creation
check would crash the process.  The import now returns the existing
`XmSYNTHETIC_NONE` result for null inputs and keeps valid resource conversion
unchanged.

The regression contract is checked by
`tests/automated/check-combobox-selected-import-input.py`.
