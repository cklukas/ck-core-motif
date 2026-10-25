# IconButton grey-stipple cache allocation

`GetGreyStipple` now handles failure to allocate its cache record by returning
`None` before dereferencing it.  The widget can therefore continue without a
stipple instead of crashing during cache initialization.

The regression check is
`tests/automated/check-iconbutton-stipple-allocation.py`.
