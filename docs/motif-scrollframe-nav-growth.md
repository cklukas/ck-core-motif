# ScrollFrame navigator-list growth failure

`_XmSFAddNavigator` now computes the next slot count before extending its
navigator list and keeps the existing allocation intact if `XtRealloc` fails.
The slot count is published only after the new list is available, so a failed
registration cannot write through null storage or leave inconsistent capacity
metadata.

The regression check is
`tests/automated/check-scrollframe-nav-growth.py`.
