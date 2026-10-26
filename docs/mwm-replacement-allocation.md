# MWM replacement allocation

`WmMalloc` is used when MWM replaces a live dynamic buffer. It now allocates
the replacement before freeing the old block. If allocation fails, the old
pointer remains valid and the caller can preserve or retry its state; the
previous order freed the old block first and left callers with a dangling
pointer.

Successful replacement behavior is unchanged. The focused source contract is
covered by `tests/automated/check-mwm-replacement-allocation.py`.
