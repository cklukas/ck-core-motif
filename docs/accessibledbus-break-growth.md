# AccessibleDBus break-array growth

Unicode text boundary calculation accumulates break positions in a dynamic
integer array.  The old macro assigned `XtRealloc`'s result directly and
continued to append even when allocation failed.

The macro now commits a replacement array only after success and jumps to its
existing cleanup path on failure, preserving valid ownership and preventing a
null-pointer write.
