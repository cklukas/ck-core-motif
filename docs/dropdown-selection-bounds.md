# DropDown selected-item tables

DropDown now checks the converted selected-item count before allocating its
temporary `XmStringTable`. An unrepresentable count reports the existing
selection error instead of wrapping the allocation size; normal text-to-list
selection behavior is unchanged.
