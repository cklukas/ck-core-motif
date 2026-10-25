# ToggleButtonGadget shadow geometry bounds

The ToggleButtonGadget shadow path now skips drawing when the gadget is no
larger than twice its highlight thickness or when either shadow GC is absent.
It also converts the unsigned rectangle dimensions before subtracting the
highlight margin.  Tiny gadgets therefore cannot wrap into oversized X draw
requests.

The focused regression is `tests/automated/check-togglebg-geometry-bounds.py`.
