# Picture accepted-text buffer growth

XmPicture parsing appends accepted characters to a state-owned string buffer.
The previous path assigned `XtRealloc`'s result directly and doubled the
capacity without checking the actual required terminator space.

Growth now computes the required length, expands until it fits, and updates
the buffer and append pointer only after allocation succeeds.  Failed growth
returns a parse failure while preserving the existing state allocation.
