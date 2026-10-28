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

Regression coverage: `tests/automated/check-drop-start-failure.py`.
