# MWM grab-mask allocation

`_WmGrabMasks` builds the modifier-mask list used by key and button grabs.
Allocation now grows the reusable list with a temporary pointer and returns
zero masks if the replacement fails, leaving the previous buffer intact. The
old path freed the existing list before allocation and then dereferenced a
null pointer after failure.

The helper also tolerates an absent lock-mask sequence and bounds the count
before adding the terminator slots. Existing successful grab behavior is
unchanged. The source contract is covered by
`tests/automated/check-mwm-grab-mask-allocation.py`.
