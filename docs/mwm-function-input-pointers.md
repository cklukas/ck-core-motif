---
nav_exclude: true
---

# MWM function-parser input pointers

The exported `ParseWmFunction` API now validates its parser-cursor and output
arguments before dereferencing either one.  A missing cursor retains the
existing invalid-function behavior by producing `F_Nop` and `F_NOP_INDEX`; a
missing output destination returns `F_NOP_INDEX` without dereferencing it.
Valid resource parsing is unchanged.

The focused regression is
`tests/automated/check-mwm-function-input-pointers.py`.
