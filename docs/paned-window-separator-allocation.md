---
nav_exclude: true
---
# PanedWindow separator allocation

When `XmPanedWindow` toggles `XmNseparatorOn`, it collects the separator
gadgets for all pane children before managing or unmanaging them.  The
temporary list now uses the full child count as its upper bound, checks the
allocation-size arithmetic against host `size_t` and Xt's `Cardinal` limits,
and verifies the allocation before collecting entries.

This fixes both the old `num_children / 3` under-allocation (which could write
past the list for valid pane layouts) and the null dereference on allocation
failure.  If the temporary list cannot be allocated, separator visibility is
left unchanged while other `SetValues` processing continues.
