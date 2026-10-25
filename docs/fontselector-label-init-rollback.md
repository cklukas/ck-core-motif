# FontSelector label initialization rollback

FontSelector now uses a checked copy helper for its built-in compound-string
labels.  A failed copy falls back to an empty compound string rather than
publishing a NULL label to its child controls and later queries.

The focused regression is
`tests/automated/check-fontselector-label-init-rollback.py`.
