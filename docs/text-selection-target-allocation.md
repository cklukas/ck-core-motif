# Text selection target-list allocation

The text selection conversion path now checks allocation of the five-atom
`MOTIF_EXPORT_TARGETS` list.  An allocation failure reports an empty integer
conversion and returns without writing through a null target-list pointer.

The focused source contract is checked by
`tests/automated/check-text-selection-target-allocation.py`.
