# DropDown selected-item conversion inputs

`SetTextFromList` converts selected List items into the DropDown text field.
The List can report a positive selected-item count without returning an item
array, and `XmStringUnparse` can fail for an individual item.  The conversion
now returns safely for a missing array and skips an item whose unparse failed,
without passing a null pointer to `mbstowcs` or changing successful item
serialization.

The public `XmDropDownGetValue` helper also rejects non-DropDown widget
handles before accessing the private text child.  Valid DropDown and
CombinationBox2 alias behavior is unchanged.

The regression contract is checked by
`tests/automated/check-dropdown-selection-inputs.py`.
