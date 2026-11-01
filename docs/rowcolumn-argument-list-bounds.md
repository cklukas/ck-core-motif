# RowColumn menu argument-list bounds

The shared RowColumn menu constructor accepts caller-supplied `ArgList`
entries. It no longer copies those entries into fixed 256-item stack arrays.
Instead, it validates null and count inputs, checks native and Xt allocation
limits, and builds dynamically sized argument lists for both the RowColumn and
any private popup shell. All temporary lists are released on every creation or
validation failure path.

The focused source contract is covered by
`tests/automated/check-rowcolumn-argument-list-bounds.py`.
