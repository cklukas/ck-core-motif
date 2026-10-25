---
nav_exclude: true
---

# MWM command window-list allocation

The MWM command protocol carries a caller-supplied count followed by window
IDs.  The command handlers now check the `count * sizeof(Window)` arithmetic
before allocating their temporary arrays.  This prevents integer wraparound
on narrow `size_t` platforms while preserving the existing command and wire
formats.  An unrepresentable list is rejected with the same safe early-return
behavior used for allocation failure.
