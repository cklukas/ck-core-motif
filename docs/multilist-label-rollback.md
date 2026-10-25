# MultiList label rollback

MultiList now copies changed find and title labels before releasing their
current `XmString` values. Failed copies preserve the existing label and skip
the child refresh; NULL find labels still fall back to the default label when
that fallback can be allocated.

The focused source regression is
`tests/automated/check-multilist-label-rollback.py`.
