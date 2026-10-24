# Mask-name allocation

`XmeGetMask` previously generated the companion mask name in a fixed 256-byte
buffer even though image names are caller-provided.  Long paths could overflow
the stack before the mask lookup; null screens and names also reached Xlib or
string handling.

The call site now rejects missing inputs, allocates enough space for the
original name plus the `_m` suffix, and releases the temporary name after the
lookup.  Existing mask naming remains unchanged.

`tests/automated/check-mask-name-allocation.py` protects this contract.
