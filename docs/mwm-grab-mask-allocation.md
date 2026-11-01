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

The implementation includes `<stdint.h>` directly for `SIZE_MAX`; this keeps
the checked path portable to toolchains whose Motif/X11 headers do not happen
to transitively provide that C definition.

The lock-mask sequence builder now also uses unsigned shift operands, rejects
unrepresentable sequence widths before deriving `2^n - 1`, and returns safely
when its initial zeroed allocation fails. The grab helper then follows its
existing absent-sequence fallback.
