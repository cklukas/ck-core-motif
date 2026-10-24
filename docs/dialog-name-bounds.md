# Dialog shell name bounds

Motif convenience dialogs create a companion shell by appending
`"_popup"` to the caller's widget name.  The old implementation added the
string lengths directly and then copied the pieces, allowing length arithmetic
to wrap before allocation for unusually long names.

`XmeCreateClassDialog` now measures the complete name with `snprintf`, allocates
the exact result plus its terminator, and performs a bounded second pass.  The
generated shell name and all public dialog behavior remain unchanged.

`tests/automated/check-dialog-name-bounds.py` protects this contract.
