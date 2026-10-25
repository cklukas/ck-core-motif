---
nav_exclude: true
---
# MainW child-list growth failures

`XmMainWindow` occasionally moves an unexpected clip-window child into its
own composite child list.  The old expansion path updated `num_slots` before
checking `XtRealloc`, so allocation failure could discard the child array and
leave the next store out of bounds.

The migration now checks slot arithmetic and both host `size_t` and Xt
`Cardinal` byte limits, commits the new pointer and count only after successful
allocation, and returns from that change-managed pass if growth cannot be
completed.
