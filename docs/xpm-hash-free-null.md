# Xpm hash cleanup null handling

`xpmHashTableFree` now returns immediately for a null table pointer before
reading its fields. Error-unwind paths can therefore call cleanup safely even
when table initialization did not complete.
