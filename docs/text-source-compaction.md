# Text-source compaction allocation

After a successful edit, the text source may shrink its gap buffer.  The
compaction path now computes the candidate capacity and commits the new
pointer, capacity, and gap bounds only after `XtRealloc` succeeds.  An
allocation or size failure leaves the existing larger buffer intact and keeps
the source usable.

The focused source contract is checked by
`tests/automated/check-text-source-compaction.py`.
