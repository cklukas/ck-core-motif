# Text drop transfer-record allocation failure

Text drop handling stores asynchronous transfer state in a temporary record.
The allocation is now checked before the record is initialized. If it fails,
the transfer is marked failed and the callback returns without dereferencing
null storage.

The regression check is
`tests/automated/check-textdrop-record-allocation.py`.
