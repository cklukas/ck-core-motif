# Drop-transfer callback destruction

The drop-transfer selection dispatcher now stops immediately when an
application transfer callback destroys its `XmDropTransferObject`. Xt marks a
destroyed widget before deferring its final teardown; continuing to advance the
transfer lists after that callback could otherwise access state that is being
retired. Normal callbacks that keep the transfer object alive retain the
existing transfer ordering and incremental-transfer behavior.

Regression coverage: `tests/automated/check-drop-transfer-callback-destruction.py`.
