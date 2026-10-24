# DropDown selected-item tables

DropDown now checks the converted selected-item count before allocating its
temporary `XmStringTable`. An unrepresentable count reports the existing
selection error instead of wrapping the allocation size; normal text-to-list
selection behavior is unchanged.

Selected-item text insertion now obtains the exact locale character count
without copying through a fixed `BUFSIZ` wide-character buffer, so long
localized entries keep correct insertion positions.
