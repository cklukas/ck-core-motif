# Label wide-string value conversion

`LabelSetValue` now checks the temporary byte-buffer allocation and the
`wcstombs` result before constructing an `XmString`. Allocation or invalid
locale conversion failure releases the input and returns without updating the
label, while valid conversions retain existing behavior.
