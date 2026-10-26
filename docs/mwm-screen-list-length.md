# MWM screen-list name lengths

`ProcessScreenListResource` now keeps screen-list name lengths in `size_t` and
checks them against the `unsigned` size accepted by `WmRealloc` before copying
the name. Both explicitly listed names and the repeated-name fallback use the
same bound, preventing truncating allocation sizes for oversized resource
values.

The focused source contract is covered by
`tests/automated/check-mwm-screen-list-length.py`.
