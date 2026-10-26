# MWM bindings property bounds

When MWM publishes a loaded binding file through Xlib, the byte count is now
kept in `size_t` and checked against Xlib's signed `int` property-length
parameter before the request is made. A missing binding string is also treated
as an unsuccessful load rather than being passed to `strlen()`.

The source contract is covered by
`tests/automated/check-mwm-bindings-property-length.py`.
