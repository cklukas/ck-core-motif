# SelectionBox restore selected items

`_XmSelectionBoxRestore` mirrors the selected List item into the text field.
The List may report a positive count without returning an item array after a
failed query or allocation.  Restore now requires both the count and a
non-null array before dereferencing the first item; otherwise it follows the
existing empty-selection path without crashing.

The regression contract is checked by
`tests/automated/check-selectionbox-restore-items.py`.
