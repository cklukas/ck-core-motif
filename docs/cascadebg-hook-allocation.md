---
nav_exclude: true
---
# CascadeButtonGadget hook allocation handling

CascadeButtonGadget's secondary-object, set-values, and get-values hooks now
check extension-object and extension-data allocations.  Partial objects are
released before returning.  Initialize, set-values, and get-values posthooks
also tolerate missing extension records when a prehook could not establish
temporary state, preventing cleanup-time null dereferences.

The regression check is `tests/automated/check-cascadebg-hook-allocation.py`.
