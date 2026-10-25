# I18List selected-row allocation

`XmI18ListGetSelectedRows` now checks its result-array allocation before
writing the terminator or selected-row pointers. Allocation failure returns
`NULL`, matching the no-selection result and avoiding a null-pointer crash;
successful selected-row enumeration is unchanged.
