---
nav_exclude: true
---
# PanedWindow separator allocation

When `XmPanedWindow` toggles `XmNseparatorOn`, it collects separator gadgets
for all pane children before managing or unmanaging them. The temporary list
now uses the full child count as its upper bound, checks allocation-size
arithmetic, and verifies the allocation before collecting entries.

This fixes the old `num_children / 3` under-allocation and the null
dereference on allocation failure. If the temporary list cannot be allocated,
separator visibility is left unchanged while other `SetValues` processing
continues.
