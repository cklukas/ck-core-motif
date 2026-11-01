# Text reflow scratch-table allocation

Text edits use temporary line tables while realigning changed lines. Its
element count is checked for multiplication and Xt's allocation limit, and
both word-wrapped and hard-line update paths retain the old scratch table
until replacement allocation succeeds. A failed growth frees only temporary
storage and leaves reflow pending for a later retry.

The focused source contract is checked by
`tests/automated/check-text-reflow-scratch-table.py`.

The non-word-wrapped growth path is additionally covered by
`tests/automated/check-text-update-line-table-allocation.py`.
