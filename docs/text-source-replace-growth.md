# Text-source replace growth

Text edits now reject position deltas that cannot be represented by the
source's `int` length before changing UI state.  The gap-growth path also
checks capacity arithmetic and commits `XtRealloc` only after success.  If
growth cannot be completed, redisplay and the insertion point are restored and
the edit is rejected without changing source contents.

The focused source contract is checked by
`tests/automated/check-text-source-replace-growth.py`.
