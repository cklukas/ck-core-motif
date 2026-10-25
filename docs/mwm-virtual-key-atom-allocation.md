---
nav_exclude: true
---
# MWM virtual-key atom allocation

The optional HP/DIN virtual-key remapping path in `mwm` interns two Motif
binding atoms.  The atom array is now checked before it is passed to
`XInternAtoms`; if the allocation cannot be satisfied, the routine releases
its replacement table and returns without changing the bindings.  Normal
startup and remapping behavior are unchanged, while allocation pressure no
longer turns this initialization path into a null-pointer write.

The source-level regression is
`tests/automated/check-mwm-virtual-key-atom-allocation.py`.
