# IconG mask-name allocation

Icon gadget mask lookup used the same fixed 255-byte mask-name buffer as the
legacy image-cache path.  Arbitrarily long icon names could overflow that
stack buffer before the mask lookup.

IconG now allocates the generated name from the caller's image-name length,
releases it after lookup, and leaves the mask unspecified if allocation fails.
Existing mask lookup and destruction semantics are unchanged.

`tests/automated/check-icon-mask-name-allocation.py` protects this contract.
