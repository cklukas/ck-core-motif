# ComboBox deletion selected-position pointer

`XmComboBoxDeletePos` reads the child List’s selected position before
repairing selection after a deletion.  A positive selected-position count does
not guarantee that the returned array pointer is available.  The repair path
now treats a missing array as no selected position, avoiding a null
dereference while preserving the existing behavior for valid List results.

The regression contract is checked by
`tests/automated/check-combobox-delete-selection-pointer.py`.
