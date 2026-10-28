# Drop-transfer callback guards

`DropTransferSelectionCB` now releases the Xt-owned result buffer when a
callback arrives without client data, then validates its callback pointers,
selection result pointers, transfer-list bounds, and current transfer entry
before dereferencing them.  It also discards a queued result if Xt has already
started destroying the transfer object. Invalid or late callback state fails
through the normal drop-transfer teardown path instead of crashing or leaving
the request pending.

Regression coverage: `tests/automated/check-drop-transfer-callback-guards.py`.
