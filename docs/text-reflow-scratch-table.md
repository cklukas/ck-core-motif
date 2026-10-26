# Text reflow scratch-table allocation

Word-wrapped Text edits use a temporary line table while realigning changed
lines.  Its element count is now checked for multiplication and Xt's
allocation limit, and a failed allocation returns before the realignment
helper sees a null table.  The existing pending-reflow state can retry later.

The focused source contract is checked by
`tests/automated/check-text-reflow-scratch-table.py`.
