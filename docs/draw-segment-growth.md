# Draw shadow segment-buffer growth

The Motif shadow drawing helper keeps a shared `XSegment` array sized for the
current border thickness.  The old path assigned `XtRealloc`'s result and
updated the capacity before checking for failure.

The growth path now checks the segment count, allocates a replacement, and
updates the shared pointer and capacity only after success.  A failed growth
returns without touching the existing drawing buffer.
