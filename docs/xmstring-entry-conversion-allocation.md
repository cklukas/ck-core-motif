# XmString entry-conversion allocation

`EntryCvtToOpt` and `EntryCvtToUnopt` now check their new-entry, text, and
rendition-array allocations. Any failed conversion releases the partial entry
and returns `NULL`, allowing callers to keep the original segment intact.
