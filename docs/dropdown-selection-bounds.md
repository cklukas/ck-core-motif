---
nav_exclude: true
---
# DropDown selected-item tables

DropDown now checks the converted selected-item count against host `size_t`
and Xt's `Cardinal` byte limits before allocating its temporary `XmStringTable`.
The checked byte count is passed explicitly at the Xt allocation boundary.
An unrepresentable count reports the existing
selection error instead of wrapping the allocation size; normal text-to-list
selection behavior is unchanged.

Selected-item text insertion now obtains the exact locale character count
without copying through a fixed `BUFSIZ` wide-character buffer, so long
localized entries keep correct insertion positions.

The converted selection-table allocation is checked before entries are copied;
allocation failure uses the existing selection error cleanup path instead of
dereferencing a null table. The source regression is
`tests/automated/check-dropdown-selection-allocation.py`.
