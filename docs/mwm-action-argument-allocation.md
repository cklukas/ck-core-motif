# MWM action-argument allocation bounds

MWM parses `f.action` arguments from its configuration file into a dynamically
grown `DtActionArg` array.  The array now has to fit both native `size_t` and
Xt's `Cardinal` allocation size before `XtRealloc` is called.  An oversized
configuration therefore stops argument growth safely instead of truncating the
requested allocation on platforms where `Cardinal` is narrower than `size_t`.

The focused source contract is covered by
`tests/automated/check-mwm-action-argument-allocation.py`.
