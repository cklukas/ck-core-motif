# ColorObj screen allocation cleanup

ColorObj allocates three per-screen arrays for colors, atoms, and usage.  If
any allocation fails, the partially allocated arrays are now released and
cleared before initialization returns.  The object remains in its normal
non-running state without retaining incomplete screen storage.

The regression check is `tests/automated/check-colorobj-screen-allocation.py`.
