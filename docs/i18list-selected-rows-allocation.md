# I18List selected-row allocation

`XmI18ListGetSelectedRows` now checks its result-array count against host
`size_t` and Xt's `Cardinal` byte limits before writing the terminator or
selected-row pointers. Allocation failure returns `NULL`, matching the
no-selection result and avoiding a null-pointer crash. The integer row-array
helper now validates its inputs and allocation size as well.
