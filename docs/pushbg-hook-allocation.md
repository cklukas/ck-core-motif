# PushButtonGadget hook allocation handling

PushButtonGadget's secondary-object, set-values, and get-values hooks now
check every extension-object and extension-data allocation.  Partial objects
are released before returning, and get-values cleanup tolerates a missing
extension record when temporary state could not be created.  Successful cache
merging and resource-hook behavior is unchanged.

The regression check is `tests/automated/check-pushbg-hook-allocation.py`.
