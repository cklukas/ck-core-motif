# List selection-copy rollback

List selection snapshots now treat each selected-item `XmStringCopy` as part of
one transaction.  If a copy fails, all earlier copies and the snapshot array
are released, and the widget publishes an empty selection snapshot rather than
passing NULL entries with a non-zero count to callbacks.

The focused regression is
`tests/automated/check-list-selection-copy-rollback.py`.
