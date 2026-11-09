# MWM `WM_STATE` write validation

`WM_STATE` is written using an unsigned X11 property field, while MWM's
writer receives its state as an `int`.  ICCCM states used by MWM are
non-negative (`WithdrawnState`, `NormalState`, and `IconicState`).  The writer
now ignores a negative caller value before the explicit conversion to the wire
field, preventing an invalid negative state from becoming an all-bits-set
property value.  Existing valid state transitions are unchanged.
