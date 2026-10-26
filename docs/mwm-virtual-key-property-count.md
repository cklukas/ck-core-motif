# MWM virtual-key property count

`VirtKeys4DIN` now validates the property-array count and Xt byte limit before
allocating Atom storage, then converts the count explicitly for Xlib's signed
`XInternAtoms` parameter. The normal two-property virtual-key update remains
unchanged while malformed future tables cannot wrap the count or allocation.

The source contract is covered by
`tests/automated/check-mwm-virtual-key-property-count.py`.
