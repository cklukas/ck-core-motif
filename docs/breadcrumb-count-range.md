# BreadcrumbBar segment-count range

`XmBreadcrumbBarSetSegments` accepts a `Cardinal` count, while the current
segment is stored and exposed through a signed `int`.  Counts above `INT_MAX`
would make the `count - 1` current-index conversion unrepresentable.  The
setter now rejects those counts before freeing or replacing existing items;
all representable counts retain their existing behavior.

The regression contract is checked by
`tests/automated/check-breadcrumb-count-range.py`.
