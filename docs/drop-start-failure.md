---
nav_exclude: true
---
# Drop-transfer start failure

`XmTransferValue` now treats a failed `XmDropTransferStart` as failed request
setup. The newly appended transfer block is removed before returning, and
`outstanding`/`count` are incremented only after a drop-transfer object exists.
This prevents a request with no possible callback from wedging the transfer
lifecycle.

Regression coverage: `tests/automated/check-drop-start-failure.py`.
