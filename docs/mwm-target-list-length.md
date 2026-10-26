# MWM TARGETS conversion length

The `TARGETS` selection response now reports the number of Atom items in
`outputLen`, as required for an X11 format-32 conversion. It no longer derives
the length from host `sizeof(Atom)`, which incorrectly doubled the count on
64-bit systems and could make consumers read beyond the allocated array.

The source contract is covered by
`tests/automated/check-mwm-target-list-length.py`.
