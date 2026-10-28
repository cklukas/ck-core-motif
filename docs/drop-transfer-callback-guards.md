# Drop-transfer callback guards

`DropTransferSelectionCB` now validates its client data, callback pointers,
selection result pointers, transfer-list bounds, and current transfer entry
before dereferencing them.  Invalid or late callback state fails through the
normal drop-transfer teardown path instead of crashing or leaving the request
pending.

Regression coverage: `tests/automated/check-drop-transfer-callback-guards.py`.
