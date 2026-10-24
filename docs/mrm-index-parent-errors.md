# Mrm index parent-update errors

`Idb__INX_EnterNodeIndex` updates the parent record number of both child
records after inserting a node entry.  The left-child update is now checked
before the result variable is reused for the right-child update.  A failure
updating either child is therefore returned to the caller instead of being
silently hidden by a later success.

The source-level regression is
`tests/automated/check-mrm-index-parent-errors.py`.
