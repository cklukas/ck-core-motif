# Class dialog allocation handling

`XmeCreateClassDialog` constructs a dialog-shell name and a temporary
argument array before creating the shell and child.  It now validates the
caller argument pointer when arguments are present, checks both allocations,
and stops cleanly if shell creation fails.  Successful dialog construction and
ownership of the temporary buffers are unchanged.

The regression check is `tests/automated/check-class-dialog-allocation.py`.
