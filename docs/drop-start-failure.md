---
nav_exclude: true
---
# Drop-transfer start failure

`XmTransferValue` now treats a failed `XmDropTransferStart` as failed request
setup. The newly appended transfer block is removed, the transfer status is
set to failure, and an already-exited transfer is finalized immediately.
When the request is made from an active destination callback, the caller
finishes the context after that callback returns. `outstanding`/`count` are
incremented only after a drop-transfer object exists, so a request with no
possible callback cannot wedge the transfer lifecycle.

The public `XmDropTransferAdd` ABI remains `void`; toolkit transfer code uses
an internal checked form that observes whether the transfer-list count grew.
If an append allocation fails, the newly created request block is rolled back
and the transfer follows the same failure teardown path.

Regression coverage: `tests/automated/check-drop-start-failure.py`.
Append-failure coverage: `tests/automated/check-drop-transfer-add-failure.py`.
