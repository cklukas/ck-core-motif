# Xm argument-filter allocation guard

`_XmFilterArgs` now checks both `size_t` and Xt `Cardinal` byte limits before
allocating a filtered argument list. If allocation fails, it leaves the
returned list NULL with a zero count, allowing callers to handle the same
empty result safely.

The focused regression is
`tests/automated/check-xm-filter-args-allocation.py`.
