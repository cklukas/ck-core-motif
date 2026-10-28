# ComboBox selected-position pointer

The ComboBox list action obtains both a selected-position count and the
selected-position array through `XtGetValues`.  A nonzero count is not enough
to make the pointer safe to dereference if the list cannot provide its array.
Navigation now requires both a positive count and a non-null position pointer;
otherwise it leaves the current selection unchanged and avoids a null
dereference.

The regression contract is checked by
`tests/automated/check-combobox-selection-pointer.py`.
