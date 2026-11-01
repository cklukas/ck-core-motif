# MWM grab-mask allocation

`_WmGrabMasks` builds the modifier-mask list used by key and button grabs.
Allocation grows the reusable list with a temporary pointer and returns zero
masks if the replacement fails, leaving the previous buffer intact. The old
path freed the existing list before allocation and then dereferenced a null
pointer after failure.

The helper tolerates an absent lock-mask sequence, bounds the count before
adding terminator slots, and verifies that the requested byte count fits both
native `size_t` arithmetic and Xt's `Cardinal` allocation interface. An
unrepresentable resource leaves the existing cache untouched. Existing
successful grab behavior is unchanged. The source contract is covered by
`tests/automated/check-mwm-grab-mask-allocation.py`.
